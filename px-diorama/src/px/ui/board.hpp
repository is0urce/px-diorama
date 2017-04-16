// name: board.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "panel.hpp"

namespace px {
	namespace ui {

		class board
			: public panel
		{
		public:
			board(color background)
				: m_background(background)
			{
			}
			board() : board({ 0, 0, 0, 1 })
			{
			}

		public:
			void set_bg_color(color background) noexcept
			{
				m_background = background;
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				window.paint(m_background);
			}

		private:
			color m_background;
		};
	}
}