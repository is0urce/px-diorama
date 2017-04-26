// name: map_chunk.hpp

#pragma once

#include "es/sprite_system.hpp"
#include "drm/es/transform_component.hpp"

#include <px/common/matrix.hpp>
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

namespace px {

	struct tile
	{
		// use transform in stack to avoid exessive management of simple anchor points
		transform_component transform;

		// two sprite to composite tiles
		es::sprite_system::unique_ptr ground;
		es::sprite_system::unique_ptr wall;

		// attributes of a tile
		rl::mass<rl::traverse> mass;
	};

	template <typename Tile>
	class map_chunk : public matrix2<Tile>
	{
	public:
		typedef Tile tile_type;

	public:
		bool traversable(point2 const& position) const noexcept
		{
			return contains(position) && at(position).mass.traversable();
		}
	};
}
