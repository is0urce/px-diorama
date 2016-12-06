// name: sprite_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "sprite.hpp"

namespace px {
	class transform_component;

	class sprite_component final
		: public sprite
		, public es::component
		, public es::link_dispatcher<sprite_component>
		, public es::link<transform_component>
	{
	public:
		virtual ~sprite_component()
		{
		}
	};
}