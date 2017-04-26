// name: player_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

namespace px {

	class transform_component;

	class player_component final
		: public es::component
		, public es::link_dispatcher<player_component>
	{
	public:
		virtual ~player_component()
		{
		}
	};
}