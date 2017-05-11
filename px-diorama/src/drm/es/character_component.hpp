// name: character_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include <px/rl/skill/skill_instance.hpp>
#include <px/rl/skill/skill_state.hpp>

namespace px {

	class character_component final
		: public es::component
		, public es::link_dispatcher<character_component>
	{
	public:

	public:
		virtual ~character_component()
		{
		}
		character_component()
		{
		}

	private:
	};

}