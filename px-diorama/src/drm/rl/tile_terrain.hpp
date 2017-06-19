// name: tile_terrain.hpp

#pragma once

#include "es/sprite_system.hpp"
#include "es/transform_component.hpp"

#include "tile_chunk.hpp"

#include <px/common/assert.hpp>
#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include "depot.hpp"

#include <fstream>
#include <cstdint>
#include <string>

namespace px {

	template <typename Tile>
	class tile_terrain
	{
	public:
		typedef Tile tile_type;

	public:
		bool traversable(point2 const& absolute) const noexcept
		{
			return m_chunk.traversable(absolute);
		}
		tile_type const& operator[](point2 const& absolute) const
		{
			return m_chunk[absolute];
		}
		void pset(point2 const& absolute, uint32_t id)
		{
			write(m_chunk[absolute], id);
		}
		void assigns_sprites(es::sprite_system * sprites) noexcept
		{
			m_sprites = sprites;
		}
		//void store(std::string const& depot_name) const
		//{
		//	std::ofstream stream(depot_name, std::ofstream::binary);
		//	m_matrix.enumerate([&](auto const& /* position */, auto const& tile) {
		//		stream.write(reinterpret_cast<char const*>(&tile.id), sizeof(tile.id));
		//	});
		//}
		//void load(std::string const& depot_name)
		//{
		//	std::ifstream stream(depot_name, std::ifstream::binary);
		//	m_matrix.enumerate([&](auto const& /* position */, auto & tile) {
		//		stream.read(reinterpret_cast<char *>(&tile.id), sizeof(tile.id));
		//		invalidate(tile);
		//	});
		//}

	public:
		tile_terrain()
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
		tile_chunk<tile_type> m_chunk;
		tile_library<tile_prototype<rl::mass<rl::traverse>>> m_library;
		es::sprite_system * m_sprites;
	};
}
