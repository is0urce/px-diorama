// name: character_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include "drm/rl/character.hpp"

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

namespace px {

	class body_component;

	class character_component final
		: public es::component
		, public es::link_dispatcher<character_component>
		, public rl::character
	{
	public:
		virtual ~character_component()
		{
			clear_skills();
		}
	};

}