// name: vfx.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// visual effects struct

#pragma once

#include "es/sprite_system.hpp"
#include "es/transform_component.hpp"

namespace px {

	struct vfx
	{
		transform_component transform;
		es::sprite_system::unique_ptr sprite;
	};
}