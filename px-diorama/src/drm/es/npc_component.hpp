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
#include <px/space/shadowcasting.hpp>

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
			auto * body = pawn->linked<body_component>();
			px_assert(pawn);
			if (pawn) {

				m_fov.recursive([&](point2 const& location) { return shell.transparent(location); }, pawn->position());

				m_target = select_target(shell);
				if (m_target) {
					m_alert = true;
					m_destination = m_target->position();
				}

				if (m_alert) {

					auto path = a_star::find(pawn->position(), m_destination, [&](point2 const& location) { return shell.traversable(location, body->traverse()); }, 50);

					if (path.size() != 0) {
						point2 & next = path.front();
						if (next != m_destination) {
							pawn->place(next);
						}
					}
				}
			}
		}
	public:
		npc_component()
			: m_alert(false)
			, m_perception_radius(10)
			, m_target(nullptr)
			, m_fov(m_perception_radius)
		{
		}
		virtual ~npc_component()
		{
		}

	private:
		transform_component * select_target(environment & shell) const
		{
			transform_component * target = nullptr;

			auto * pawn = linked<transform_component>();
			auto * body = pawn ? pawn->linked<body_component>() : nullptr;
			auto * space = pawn ? pawn->world() : nullptr;

			px_assert(body);
			px_assert(space);

			if (body && space) {

				point2 position = pawn->position();
				unsigned int distance = m_perception_radius;

				space->find(position.x(), position.y(), m_perception_radius, [&](int /*x*/, int /*y*/, transform_component * element) {
					auto * target_body = element->linked<body_component>();

					if (m_fov.in_sight(element->position())) {
						if (target_body->hostile(*body)) {

							if (shell.distance(position, element->position()) < distance) {
								target = element;
							}
						}
					}
				});
			}

			return target;
		}

	private:
		bool m_alert;
		unsigned int m_perception_radius;
		transform_component * m_target;
		point2 m_destination;
		shadowcasting m_fov;
	};
}