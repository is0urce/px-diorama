// name: skill_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/common/assert.hpp>
#include <px/ui/panel.hpp>
#include "../es/character_component.hpp"

#include <stdexcept>

namespace px {
	namespace ui {

		class skill_panel
			: public panel
		{
		public:

		public:
			virtual ~skill_panel()
			{
			}
			skill_panel(target_panel * user, target_panel * target)
				: m_user(user)
				, m_target(target)
			{
				if (!user) throw std::runtime_error("px::ui::skill_panel::ctor() - user is null");
				if (!target) throw std::runtime_error("px::ui::skill_panel::ctor() - user is null");
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				px_assert(m_user);
				px_assert(m_target);

				body_component * user = body(*m_user);
				character_component * character = user ? user->linked<character_component>() : nullptr;

				int width = 5;
				int offset = 0;
				point2 range(width, 1);

				if (character) {
					size_t size = character->count_skills();
					for (size_t i = 0; i != size; ++i) {
						auto const* instance = character->get_skill(i);

						bool useable = instance && (instance->targeted() ? instance->useable(user, body(*m_target)) : instance->useable(user, m_target->position()));

						point2 button{ offset, 0 };

						std::string text = std::to_string(i + 1) + instance->state().alias();

						window.paint({ button, range }, useable ? 0x666666 : 0x333333);
						window.paint({ button, {1, 1} }, useable ? 0x00ff00 : 0xff0000);
						window.print_n(button, 0xffffff, text, width);

						offset += 1 + range.x();
					}
				}
			}
			virtual bool click_panel(point2 const& /* position */, int /* button */) override
			{
				return true;
			}

		private:
			body_component * body(target_panel & lock) const
			{
				transform_component * pawn = lock.target();
				return pawn ? pawn->linked<body_component>() : nullptr;
			}

		private:
			target_panel *	m_user;
			target_panel *	m_target;
		};
	}
}