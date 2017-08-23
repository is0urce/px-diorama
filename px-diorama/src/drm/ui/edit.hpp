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
				: text("")
			{
			}
			virtual ~edit()
			{
			}

		protected:
			virtual bool print_panel(unsigned int codepoint) override
			{
				if (input_focused()) {
					set_text(current() + string::code_to_utf8(codepoint));
					return true;
				}

				return false;
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
		};
	}
}