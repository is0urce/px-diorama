// name: tile_terrain.hpp
// type: c++
// auth: is0urce
// desc: class template

#pragma once

#include "settings.hpp"

#include "tile_prototype.hpp"
#include "tile_library.hpp"
#include "tile_instance.hpp"
#include "tile_surface.hpp"

#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include <px/fn/cellular_automata.hpp>

#include "depot.hpp"

#include <fstream>
#include <memory>
#include <random>
#include <string>

namespace px {

	template <typename Tile>
	class tile_terrain
	{
	public:
		typedef Tile tile_type;
		typedef stream<matrix2<tile_type, cell_width, cell_height>> chunk_type;

	public:
		bool traversable(point2 const& absolute) const noexcept
		{
			return m_surface.get_or(absolute, m_border).mass.traversable();
		}
		tile_type const& operator[](point2 const& absolute) const
		{
			return m_surface.get_or(absolute, m_border);
		}
		void pset(point2 const& absolute, uint32_t id)
		{
			//setup(m_chunk[absolute], id);
		}
		void assigns_sprites(es::sprite_system * sprites) px_noexcept
		{
			px_assert(sprites);
			m_sprites = sprites;
		}
		void store(point2 const& cell) const
		{
		//	std::string depot_name = "level.map";
		//	std::ofstream stream(depot_name, std::ofstream::binary);

		//	px_assert(m_chunk);
		//	if (m_chunk) {
		//		(*m_chunk)->enumerate([&](auto const& /* position */, auto const& tile) {
		//			stream.write(reinterpret_cast<char const*>(&tile.id), sizeof(tile.id));
		//		});
		//	}

		}
		void load(point2 const& cell)
		{
		//	std::string depot_name = "level.map";
		//	std::ifstream stream(depot_name, std::ifstream::binary);

		//	px_assert(m_chunk);
		//	if (m_chunk) {
		//		(*m_chunk)->enumerate([&](auto const& /* position */, auto & tile) {
		//			stream.read(reinterpret_cast<char *>(&tile.id), sizeof(tile.id));
		//			invalidate(tile);
		//		});
		//	}
		}
		void generate_chunk(point2 const& cell, matrix2<tile_type, cell_width, cell_height> & map)
		{
			std::mt19937 rng;
			fn::cellular_automata<unsigned char, cell_width, cell_height> automata([&](size_t /*x*/, size_t /*y*/) -> unsigned char { return rng() % 2; });
			automata.mutate<unsigned char>(4, 0, [](auto acc, auto cell) -> unsigned char { return acc + cell; }, [](auto x) -> unsigned char { return (x >= 5) ? 1 : 0; });

			map.enumerate([&](auto const& relative, auto & tile) {
				switch (automata->at(relative)) {
				case 1: {
					setup(tile, (std::rand() % 3 == 0) ? 2 : 3);
					break;
				}
				default:
					setup(tile, 1);
					break;
				}
			});

			point2 offset = cell * point2(cell_width, cell_height);
			map.enumerate([&](auto const& relative, auto & tile) {
				tile.transform.move(relative + offset);
				tile.transform.store_position();
			});
		}
		void clear()
		{
			m_surface.clear();
		}
		void focus(point2 const& absolute_world)
		{
			m_surface.focus(absolute_world);
			m_surface.load([&](point2 const absolute_cell, auto & stream) {

				stream.load([this, absolute_cell](auto & map) {
					generate_chunk(absolute_cell, map);
				});
			});
		}

	public:
		tile_terrain()
			: m_sprites(nullptr)
		{
			m_library.load(depot::load_document(tiles_path)["tiles"]);
			//m_chunk = std::make_unique<chunk_type>();
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
		tile_type m_border;
		//std::unique_ptr<chunk_type> m_chunk;
		tile_surface<tile_type, cell_width, cell_height, 0> m_surface;
		tile_library<tile_prototype<rl::mass<rl::traverse>>> m_library;
		es::sprite_system * m_sprites;
	};
}
