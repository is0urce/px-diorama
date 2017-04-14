// name: board.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "panel.hpp"

namespace px
{
	namespace ui
	{
		class board
			: public panel
		{
		public:
			board(color background)
				: m_back(background)
			{
			}
			board() : board({ 0, 0, 0, 0 })
			{
			}

		public:
			void set_bg_color(color background) noexcept
			{
				m_back = background;
			}

		protected:
			virtual void draw_panel(display & window) const
			{
				window.paint(m_back);
			}

		private:
			color m_back;
		};
	}
}