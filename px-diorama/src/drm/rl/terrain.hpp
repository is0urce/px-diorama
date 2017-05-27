// name: terrain.hpp

#pragma once

#include "es/sprite_system.hpp"
#include "drm/es/transform_component.hpp"

#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include "tile_library.hpp"
#include "../configuration.hpp"

#include <json.hpp>

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
		tile_type & operator[](point2 const& position)
		{
			return m_matrix[position];
		}
		void pset(point2 const& position, uint32_t id)
		{
			auto & tile = m_matrix[position];
			auto & prototype = m_library[id];
			tile.mass = prototype.mass;

			if (m_sprites) {
				tile.ground = m_sprites->make_unique(prototype.background);
				tile.ground->connect(&tile.transform);
				tile.ground->activate();
			}
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

	public:
		terrain_chunk()
		{
			auto config = nlohmann::json::parse(std::ifstream(tiles_path));
			m_library.load(config["tiles"]);
		}

	private:
		matrix2<tile_type> m_matrix;
		tile_library<tile_attributes<rl::mass<rl::traverse>>> m_library;
		es::sprite_system * m_sprites;
	};
}
