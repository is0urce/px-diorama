#pragma once

#include <px/common/matrix.hpp>
#include <px/common/shared_ptr.hpp>

#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

namespace px {

	class transform_component;
	class sprite_component;


	struct tile : public rl::mass<rl::traverse>
	{
		shared_ptr<transform_component> transform;
		shared_ptr<sprite_component> sprite;
	};

	template <typename Tile>
	class map_chunk : public matrix2<Tile>
	{
	public:
		typedef Tile tile_type;

	public:
		bool traversable(point2 const& position) const noexcept
		{
			return contains(position) && at(position).traversable();
		}
	};
}
