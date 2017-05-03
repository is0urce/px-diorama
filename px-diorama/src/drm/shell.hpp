// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// input manager facade and application level event dispatcher

#pragma once

#include "key_translator.hpp"
#include "perception.hpp"
#include "environment.hpp"

namespace px {

	static const unsigned int gui_cell_width = 16;
	static const unsigned int gui_cell_height = 16;

	class shell
		: public key_translator<shell>
		, public environment
	{
	public:
		void press(key action)
		{
			bool processed = ui()->press(static_cast<unsigned int>(action));

			if (!processed) {
				translate_key(action);
			}
		}
		void text(unsigned int /* codepoint */)
		{
		}
		void hover(int x, int y)
		{
			target(translate_world(m_hover = { x, y }));
		}
		void click(int button)
		{
			bool processed = ui()->click(translate_gui(m_hover), button);

			if (!processed)
			{
				activate(0);
			}
		}
		void scroll(double vertical, double horisontal)
		{
			m_pixel_zoom += (vertical + horisontal < 0) ? -1 : 1;
			m_pixel_zoom = std::max(1, std::min(m_pixel_zoom, 8));

			pixel_snap();
		}
		void pixel_snap(unsigned int ppu = 32)
		{
			m_perception.set_scale(ppu * m_pixel_zoom * 2.0f / m_screen_width);
		}

		perception const& view() const noexcept
		{
			return m_perception;
		}
		void frame(double time)
		{
			// clear ui grid
			m_perception.canvas().cls();
			ui()->layout(m_perception.canvas().range());

			update(m_perception, time);
		}
		void resize(unsigned int width, unsigned int height)
		{
			m_screen_width = width;
			m_screen_height = height;
			m_perception.canvas().resize(width / gui_cell_width, height / gui_cell_height);
			pixel_snap();
		}

	public:
		shell()
			: m_pixel_zoom(2)
		{
			pixel_snap();
		}

	private:
		point2 translate_gui(point2 pixel_coordinates) const
		{
			return pixel_coordinates / point2(gui_cell_width, gui_cell_height);
		}
		point2 translate_world(point2 pixel_coordinates) const
		{
			vector2 position = pixel_coordinates / vector2(m_screen_width, m_screen_height); // (0, 1) screen space

			// (0, 1) -> (-1, +1) and reverse y
			position.move(vector2{ -0.5, -0.5 });
			position.multiply(vector2{ 2.0, -2.0 * m_screen_height / m_screen_width }); // account aspect
			
			position /= m_perception.scale();

			return position.floor();
		}

	private:
		perception m_perception;

		// ui transforms
		point2 m_hover;
		unsigned int m_screen_width;
		unsigned int m_screen_height;
		unsigned int m_ui_width;
		unsigned int m_ui_height;
		int m_pixel_zoom;
	};
}