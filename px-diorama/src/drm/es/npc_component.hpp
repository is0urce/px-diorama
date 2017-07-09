// name: npc_component.hpp
// type: c++
// auth: is0urce
// desc: class

#pragma once

#include "drm/environment.hpp"

#include <px/common/assert.hpp>
#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include <px/space/a_star.hpp>

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
		void act(environment & shell)
		{
			auto * pawn = linked<transform_component>();
			auto * body = pawn ? pawn->linked<body_component>() : nullptr;
			auto * space = pawn ? pawn->world() : nullptr;

			px_assert(pawn);
			px_assert(space);

			point2 position = pawn->position();
			transform_component * target = nullptr;

			space->find(position.x(), position.y(), 10, [&](int /*x*/, int /*y*/, transform_component * element) {
				auto * target_body = element->linked<body_component>();

				if (target_body->hostile(*body)) {
					target = element;
				}
			});

			if (target) {

				point2 destination = target->position();

				auto list = a_star::find(position, destination, [&](point2 const& location) { return shell.traversable(location); }, 50);

				if (list.size() != 0) {
					pawn->place(list.front());
				}
			}
		}

	public:
		npc_component()
			: m_alert(false)
		{
		}
		virtual ~npc_component()
		{
		}

	private:
		bool m_alert;
	};
}