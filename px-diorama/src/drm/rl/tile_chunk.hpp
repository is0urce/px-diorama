// name: tile_chunk.hpp
// type: c++
// auth: is0ruce
// desc: class template

#pragma once

#include "settings.hpp"
#include "es/sprite_system.hpp"
#include "es/transform_component.hpp"

#include "tile_prototype.hpp"
#include "tile_library.hpp"
#include "tile_instance.hpp"

#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include "depot.hpp"

#include <px/fn/dig_generator.hpp>
#include <px/fn/cellular_automata.hpp>

#include <fstream>
#include <cstdint>
#include <string>

namespace px {

	template <typename Tile>
	class tile_chunk
	{
	public:
		typedef Tile tile_type;

	public:
		bool traversable(point2 const& position) const noexcept
		{
			return m_matrix.contains(position) && m_matrix[position].mass.traversable();
		}
		tile_type const& operator[](point2 const& relative) const
		{
			return m_matrix[relative];
		}
		void pset(point2 const& relative, uint32_t id)
		{
			write(m_matrix[relative], id);
		}
		void assigns_sprites(es::sprite_system * sprites) noexcept
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

			//fn::dig_generator dig(range.x(), range.y());
			//dig.generate(std::mt19937{}, 4, 7, 1, 15);

			std::mt19937 rng;
			fn::cellular_automata<unsigned char, 100, 100> automata([&](size_t /*x*/, size_t /*y*/) -> unsigned char { return rng() % 2; });
			automata.mutate<unsigned char>(4, 0
				, [](auto acc, auto cell) -> unsigned char { return acc + cell; }
				, [](auto x) -> unsigned char { return (x >= 5) ? 1 : 0; });

			//matrix2<unsigned char> digged_map(range);
			//dig.rasterize(digged_map);

			m_matrix.enumerate([&](auto const& point, auto & tile) {
				switch (automata->at(point)) {
				case 1: {
					setup(tile, std::rand() % 3 == 0 ? 2 : 3);
					//setup(tile, 1);
					break;
				}
				default:
					setup(tile, 1);
					//setup(tile, std::rand() % 3 == 0 ? 2 : 3);
					break;
				}
			});
		}
		void store(std::string const& depot_name) const
		{
			std::ofstream stream(depot_name, std::ofstream::binary);
			m_matrix.enumerate([&](auto const& /* position */, auto const& tile) {
				stream.write(reinterpret_cast<char const*>(&tile.id), sizeof(tile.id));
			});
		}
		void load(std::string const& depot_name)
		{
			std::ifstream stream(depot_name, std::ifstream::binary);
			m_matrix.enumerate([&](auto const& /* position */, auto & tile) {
				stream.read(reinterpret_cast<char *>(&tile.id), sizeof(tile.id));
				invalidate(tile);
			});
		}

	public:
		tile_chunk()
			: m_sprites(nullptr)
		{
			m_library.load(depot::load_document(tiles_path)["tiles"]);
		}

	private:
		void invalidate(tile_type & tile) const
		{
			auto const& prototype = m_library.at(tile.id);

			tile.mass = prototype.mass;
			px_assert(m_sprites);
			if (m_sprites) {
				if (!tile.ground || tile.ground->name != prototype.background) {
					tile.ground = m_sprites->make_unique(prototype.background);
					px_assert(tile.ground);
					if (tile.ground) {
						tile.ground->connect(&tile.transform);
						tile.ground->activate();
					}
				}
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
