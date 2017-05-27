// name: terrain.hpp

#pragma once

#include "configuration.hpp"
#include "es/sprite_system.hpp"
#include "es/transform_component.hpp"

#include "tile_prototype.hpp"
#include "tile_library.hpp"
#include "tile_instance.hpp"

#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include <json.hpp>

#include "fn/generator.hpp"
#include <px/fn/bsp.hpp>

#include <cstdint>
#include <fstream>
#include <string>

namespace px {

	template <typename Tile>
	class terrain_chunk
	{
	public:
		typedef Tile tile_type;

	public:
		bool traversable(point2 const& position) const noexcept
		{
			return m_matrix.contains(position) && m_matrix[position].mass.traversable();
		}
		tile_type const& operator[](point2 const& position) const
		{
			return m_matrix[position];
		}
		void pset(point2 const& position, uint32_t id)
		{
			write(m_matrix[position], id);
		}
		void assigns_sprites(es::sprite_system * sprites)
		{
			m_sprites = sprites;
		}
		void resize(point2 const& range)
		{
			m_matrix.resize(range);
			m_matrix.enumerate([this](auto const& point, auto & tile) {
				tile.transform.move(point);
				tile.transform.store_position();
			});
		}
		void generate()
		{
			point2 range = m_matrix.range();

			fn::dig_generator dig(range.x(), range.y());
			dig.generate(std::mt19937{}, 4, 7, 1, 15);

			matrix2<unsigned char> digged_map(range);
			dig.rasterize(digged_map);

			m_matrix.enumerate([&](auto const& point, auto & tile) {
				switch (digged_map[point]) {
				case 1: {
					setup(tile, 1);
					break;
				}
				default:
					setup(tile, std::rand() % 3 == 0 ? 2 : 3);
					break;
				}
			});
		}
		void dump(std::string const& name) const
		{
			std::ofstream stream(name, std::ofstream::binary);
			m_matrix.enumerate([&](auto const& /* position */, auto const& tile) {
				stream.write(reinterpret_cast<char const*>(&tile.id), sizeof(tile.id));
			});
		}
		void load(std::string const& name)
		{
			std::ifstream stream(name, std::ifstream::binary);
			m_matrix.enumerate([&](auto const& /* position */, auto & tile) {
				stream.read(reinterpret_cast<char *>(&tile.id), sizeof(tile.id));
				invalidate(tile);
			});
		}

	public:
		terrain_chunk()
		{
			auto config = nlohmann::json::parse(std::ifstream(tiles_path));
			m_library.load(config["tiles"]);
		}

	private:
		void invalidate(tile_type & tile) const
		{
			auto & prototype = m_library.at(tile.id);

			tile.mass = prototype.mass;

			px_assert(m_sprites);
			if (m_sprites) {
				tile.ground = m_sprites->make_unique(prototype.background);
				tile.ground->connect(&tile.transform);
				tile.ground->activate();
			}
		}
		void setup(tile_type & tile, uint32_t id) const
		{
			tile.id = id;
			invalidate(tile);
		}

	private:
		matrix2<tile_type> m_matrix;
		tile_library<tile_prototype<rl::mass<rl::traverse>>> m_library;
		es::sprite_system * m_sprites;
	};
}
