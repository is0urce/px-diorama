// name: sprite_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link.hpp>

#include "sprite.hpp"

namespace px {
	class transform_component;
	class sprite_component
		: public sprite
		, public es::component
		, public link_dispatcher<sprite_component>
		, public link<transform_component>
	{

	};
}