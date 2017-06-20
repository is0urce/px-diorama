// name: tile_instance.hpp

#pragma once

#include "es/sprite_system.hpp"
#include "es/transform_component.hpp"

#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

#include <cstdint>

namespace px {

	struct tile_instance
	{
		uint32_t						id;			// identifier for serialization
		rl::mass<rl::traverse>			mass;		// attributes of a tile
		transform_component				transform;	// use transform on stack to avoid exessive management of simple anchor points
		es::sprite_system::unique_ptr	ground;		// background of composite sprite
		es::sprite_system::unique_ptr	wall;		// foreground of composite sprite
	};

}