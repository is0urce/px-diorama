// name: terrain.hpp

#pragma once

#include "es/sprite_system.hpp"
#include "drm/es/transform_component.hpp"

#include "tile_prototype.hpp"
#include "tile_library.hpp"
#include "../configuration.hpp"

#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include <json.hpp>

#include "fn/generator.hpp"
#include <px/fn/bsp.hpp>

namespace px {

	struct tile
	{
		// use transform on stack to avoid exessive management of simple anchor points
		transform_component transform;

		// two sprites to composite tiles
		es::sprite_system::unique_ptr ground;
		es::sprite_system::unique_ptr wall;

		// attributes of a tile
		rl::mass<rl::traverse> mass;
	};

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
					write(tile, 1);
					break;
				}
				default:
					write(tile, std::rand() % 3 == 0 ? 2 : 3);
					break;
				}
			});
		}

	public:
		terrain_chunk()
		{
			auto config = nlohmann::json::parse(std::ifstream(tiles_path));
			m_library.load(config["tiles"]);
		}

	private:
		void write(tile_type & tile, uint32_t id) const
		{
			auto & prototype = m_library.at(id);

			tile.mass = prototype.mass;

			px_assert(m_sprites);
			if (m_sprites) {
				tile.ground = m_sprites->make_unique(prototype.background);
				tile.ground->connect(&tile.transform);
				tile.ground->activate();
			}
		}

	private:
		matrix2<tile_type> m_matrix;
		tile_library<tile_prototype<rl::mass<rl::traverse>>> m_library;
		es::sprite_system * m_sprites;
	};
}
