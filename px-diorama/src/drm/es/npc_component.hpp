// name: npc_component.hpp
// type: c++
// auth: is0urce
// desc: class

#pragma once


//#include "drm/environment.hpp"

#include <px/common/assert.hpp>
#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

namespace px {

	class transform_component;
	class character_component;

	class npc_component final
		: public es::component
		, public es::link_dispatcher<npc_component>
		, public es::link<transform_component>
		, public es::link<character_component>
	{
	public:
		void act()
		{
			transform_component * pawn = linked<transform_component>();
			px_assert(pawn);
			if (pawn) {
				pawn->move({ 1, 0 });
			}
		}

	public:
		virtual ~npc_component()
		{
		}
	};
}