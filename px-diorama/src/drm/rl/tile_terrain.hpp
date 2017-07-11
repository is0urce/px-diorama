// name: tile_terrain.hpp
// type: c++
// auth: is0urce
// desc: class template

#pragma once

#include "settings.hpp"

#include "terrain_generator.hpp"

#include "depot.hpp"

#include "tile_prototype.hpp"
#include "tile_library.hpp"
#include "tile_instance.hpp"
#include "tile_surface.hpp"

#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include <fstream>
#include <functional>
#include <memory>
#include <string>

namespace px {

	template <typename Tile>
	class tile_terrain
	{
	public:
		typedef Tile tile_type;
		typedef tile_prototype<rl::mass<rl::traverse>> blueprint_type;
		typedef tile_surface<tile_type, cell_width, cell_height, 1> surface_type;
		typedef std::function<void(point2 const&)> callback_fn;

	public:
		bool traversable(point2 const& absolute, rl::traverse_options<rl::traverse> traverse) const noexcept
		{
			return m_surface.get_or(absolute, m_border).mass.traversable(traverse);
		}
		bool transparent(point2 const& absolute) const noexcept
		{
			return m_surface.get_or(absolute, m_border).mass.transparent();
		}
		tile_type const& operator[](point2 const& absolute) const
		{
			return m_surface.get_or(absolute, m_border);
		}
		void pset(point2 const& absolute, uint32_t id)
		{
			auto & tile = m_surface.get_or(absolute, m_border);
			if (&tile != &m_border) {
				setup(tile, id);
			}
		}
		void provide_sprites(es::sprite_system * sprites) px_noexcept
		{
			px_assert(sprites);
			m_sprites = sprites;
		}
		void generate_chunk(point2 const& cell, matrix2<tile_type, cell_width, cell_height> & map)
		{
			auto terrain_data = terrain_generator<cell_width, cell_height>::generate_cavern();

			point2 offset = cell * point2(cell_width, cell_height);
			map.enumerate([&](point2 const& relative, auto & tile) {
				setup(tile, terrain_data[relative]);
				tile.transform.move(relative + offset);
				tile.transform.store_position();
			});
		}
		void clear()
		{
			m_surface.clear();
		}
		void wait()
		{
			m_surface.wait();
		}
		void focus(point2 const& absolute_world)
		{
			m_surface.focus(absolute_world);
			m_surface.load([this](point2 const absolute_cell, auto & stream) {
				stream.load_stream([this, absolute_cell](auto & map) {
					generate_chunk(absolute_cell, map);
				});
			});
		}
		void dump()
		{
			m_surface.enumerate([this](point2 cell, auto const& map) {
				store(cell, map);
			});
		}
		void load(point2 const& cell)
		{
			auto * chunk = m_surface.get_chunk(cell);

			px_assert(m_chunk);
			if (m_chunk) {
				load(cell, *chunk);
			}
		}
		void store(point2 const& cell) const
		{
			auto const* chunk = m_surface.get_chunk(cell);

			px_assert(m_chunk);
			if (chunk) {
				store(cell, *chunk);
			}
		}

		void on_enter(callback_fn && load_fn)
		{
			m_surface.on_enter(std::forward<callback_fn>(load_fn));
		}
		void on_leave(callback_fn && leave_fn)
		{
			m_surface.on_leave(std::forward<callback_fn>(leave_fn));
		}

	public:
		tile_terrain()
			: m_sprites(nullptr)
		{
			m_library.load(depot::load_document(tiles_path)["tiles"]);
			m_border.mass.make_opaque();
			m_border.mass.make_blocking();
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
		std::string depot_name(point2 const& cell) const
		{
			int x_bias = 500 + cell.x();
			int y_bias = 500 + cell.y();
			return std::string(terrain_directory) + "level_" + std::to_string(x_bias) + "_" + std::to_string(y_bias) + ".map";
		}
		template <typename Matrix>
		void store(point2 const& cell, Matrix const& map) const
		{
			std::string name = depot_name(cell);
			std::ofstream stream(name, std::ios_base::binary);
			if (!stream.is_open()) throw std::runtime_error("px::tile_terrain::store(...) - can't open file " + name);

			map.enumerate([&](auto const& /* position */, auto const& tile) {
				stream.write(reinterpret_cast<char const*>(&tile.id), sizeof(tile.id));
			});
		}
		template <typename Matrix>
		void load(point2 const& cell, Matrix & map)
		{
			std::string name = depot_name(cell);
			std::ifstream stream(name, std::ios_base::binary);
			if (!stream.is_open()) throw std::runtime_error("px::tile_terrain::load(...) - can't open file " + name);

			map.enumerate([&](auto const& /* position */, auto & tile) {
				stream.read(reinterpret_cast<char *>(&tile.id), sizeof(tile.id));
				invalidate(tile);
			});
		}

	private:
		tile_surface<tile_type, cell_width, cell_height, 1>	m_surface;		// surface data streams
		tile_type											m_border;		// fallback value
		tile_library<blueprint_type>						m_library;		// tile protorypes
		es::sprite_system *									m_sprites;		// sprite factory
	};
}
