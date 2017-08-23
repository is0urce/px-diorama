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
			virtual ~edit()
			{
			}
			edit()
				: text("")
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
				if (input_focused()) {
					switch (static_cast<key>(action_code)) {
					case key::command_cancel:
					case key::command_ok:
						release_input_focus();
						break;
					case key::command_backspace: {
						auto str = current();
						auto len = str.length();
						set_text(len > 0 ? str.substr(0, len - 1) : "");
					}
						break;
					}
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
			virtual std::string format_text() const override
			{
				if (input_focused()) {
					return text::format_text() + "_";
				}
				return text::format_text();
			}
		};
	}
}