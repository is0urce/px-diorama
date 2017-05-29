// name: skill_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>
#include "../es/character_component.hpp"

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
			skill_panel()
			{
			}

		protected:
			virtual void draw_panel(display & /* window */) const override
			{
			}
			virtual bool click_panel(point2 const& /* position */, int /* button */) override
			{
				return true;
			}

		private:
			character_component m_character;
		};
	}
}