// name: terrain.hpp

#pragma once

#include "es/sprite_system.hpp"
#include "drm/es/transform_component.hpp"

#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

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

	private:
		matrix2<tile_type> m_matrix;
	};
}
