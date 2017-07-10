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
			auto * character = linked<character_component>();

			if (!body->alive()) return;

			px_assert(pawn);
			px_assert(character);
			if (pawn && character) {

				lock_target(shell);

				size_t skill_count = character->count_skills();

				bool cast = false;
				if (m_target) {
					auto * target_body = m_target->linked<body_component>();
					for (size_t slot = 0; slot != skill_count; ++slot) {
						auto * skill = character->get_skill(slot);
						if (skill->targeted() && skill->useable(body, target_body)) {
							skill->use(body, target_body);
							cast = true;
							break;
						}
					 }
				}

				if (m_alert && !cast) {

					auto path = a_star::find(pawn->position(), m_destination, [&](point2 const& location) { return shell.traversable(location, body->traverse()); }, 150);

					if (path.size() != 0) {
						point2 & next = path.front();
						if (shell.traversable(next, body->traverse())) {
							pawn->place(next);

							// found
							if (!m_target) {
								lock_target(shell);
							}
						}
					}
				}
			}
		}

		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(m_alert);
			archive(m_destination);
		}

	public:
		npc_component()
			: m_alert(false)
			, m_radius(10)
			, m_target(nullptr)
			, m_fov(m_radius)
		{
		}
		virtual ~npc_component()
		{
		}

	private:
		transform_component * select_target(environment const& shell) const
		{
			transform_component * target = nullptr;

			auto * pawn = linked<transform_component>();
			auto * body = pawn ? pawn->linked<body_component>() : nullptr;
			auto * space = pawn ? pawn->world() : nullptr;

			px_assert(body);
			px_assert(space);

			if (body && space) {

				point2 position = pawn->position();
				unsigned int distance = m_radius;

				space->find(position.x(), position.y(), m_radius, [&](int /*x*/, int /*y*/, transform_component * element) {
					if (element) {
						if (m_fov.in_sight(element->position())) {
							auto * target_body = element->linked<body_component>();
							if (target_body && target_body->alive() && target_body->hostile(*body)) {

								if (shell.distance(position, element->position()) < distance) {
									target = element;
								}
							}
						}
					}
				});
			}

			return target;
		}
		void lock_target(environment const& shell)
		{
			auto * pawn = linked<transform_component>();
			if (pawn) {

				m_fov.reset();
				m_fov.recursive([&](point2 const& location) { return shell.transparent(location); }, pawn->position());

				m_target = select_target(shell);
				if (m_target) {
					m_alert = true;
					m_destination = m_target->position();
				}
			}
		}

	private:
		bool m_alert;
		unsigned int m_radius;
		transform_component * m_target;
		point2 m_destination;
		shadowcasting m_fov;
	};
}