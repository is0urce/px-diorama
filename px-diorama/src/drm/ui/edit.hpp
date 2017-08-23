// name: edit.hpp
// type: c++
// desc: class
// auth: isource

// widget for text editing

#pragma once

#include <px/ui/panel.hpp>
#include <px/ui/text.hpp>

#include "drm/key.hpp"

namespace px {
	namespace ui {

		class edit
			: public text
		{
		public:
			edit()
				: text([]() { return ""; }, { 0, 0, 0, 1 }, text_alignment::left)
			{
			}
			virtual ~edit()
			{
			}

		protected:
			virtual bool print_panel(unsigned int codepoint) override
			{
				if (input_focused()) {
					auto str = m_text();
					set_text(str + string::code_to_utf8(codepoint));
				}
			}
			virtual bool press_panel(unsigned int action_code) override
			{
				key action = static_cast<key>(action_code);
				if (action == key::command_cancel || action == key::command_ok) {
					release_input_focus();
					return true;
				}

				return false;
			}
			virtual bool click_panel(point2 const& /* position */, int button) override
			{
				if (button == 0) {
					request_input_focus();
					return true;
				}

				return false;
			}

		private:
			color m_color;
			text_alignment m_alignment;
			text_fn m_text;
		};
	}
}