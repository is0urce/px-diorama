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

		public:
			button(click_fn click)
				: m_click(click)
			{
			}
			virtual ~button()
			{
			}

		protected:
			virtual bool click_panel(point2 const& /*position*/, int button) override
			{
				m_click(button);
			}

		private:
			click_fn m_click;
		};
	}
}