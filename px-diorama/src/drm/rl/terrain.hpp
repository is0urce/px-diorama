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
		void resize(point2 const& size)
		{
			m_matrix.resize(size);
		}
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
			m_matrix[position].mass = m_library[id].mass;
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
	};
}
