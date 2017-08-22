// name: button.hpp
// type: c++
// desc: class
// auth: isource

// button widget

#pragma once

#include "panel.hpp"

#include <functional>

namespace px {
	namespace ui {

		class button
			: public panel
		{
		public:
			typedef std::function<void(int)> click_fn;
			typedef std::function<void()> hover_fn;

		public:
			void on_click(click_fn click)
			{
				m_click = click;
			}
			void on_hover(hover_fn hover_action)
			{
				m_hover = hover_action;
			}
			void register_shortcut(unsigned int code)
			{
				m_shortcuts.push_back(code);
			}

		public:
			virtual ~button()
			{
			}
			button()
			{
			}
			button(click_fn click)
				: m_click(click)
			{
			}

		protected:
			virtual bool click_panel(point2 const& /*position*/, int button) override
			{
				press_button(button);
				return true;
			}
			virtual bool hover_panel(point2 const& /*position*/) override
			{
				hover_button();
				return true;
			}
			virtual bool press_panel(unsigned int code) override
			{
				for (auto shortcut : m_shortcuts) {
					if (shortcut == code) {
						press_button(0);
						return true;
					}
				}
				return false;
			}

		private:
			void press_button(int button)
			{
				if (m_click) {
					m_click(button);
				}
			}
			void hover_button()
			{
				if (m_hover) {
					m_hover();
				}
			}

		private:
			click_fn					m_click;
			hover_fn					m_hover;
			std::vector<unsigned int>	m_shortcuts;
		};
	}
}