// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// input manager facade and application level event dispatcher

#pragma once

#include "key_translator.hpp"
#include "perception.hpp"
#include "environment.hpp"

#include <px/ui/panel.hpp>

namespace px {

	static const unsigned int gui_cell_width = 16;
	static const unsigned int gui_cell_height = 16;
	static const unsigned int zoom_default = 2;

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
			bool processed = ui()->hover(translate_gui(m_hover));

			if (!processed) {
				target(translate_world(m_hover = { x, y }));
			}	
		}
		void click(int button)
		{
			bool processed = ui()->click(translate_gui(m_hover), button);

			if (!processed) {
				activate(0);
			}
		}
		void scroll(double horisontal, double vertical)
		{
			bool processed = ui()->scroll(horisontal, vertical);

			if (!processed) {
				m_pixel_zoom += (vertical + horisontal < 0) ? -1 : 1;
				m_pixel_zoom = std::max(1, std::min(m_pixel_zoom, 8));

				pixel_snap();
			}
		}
		void pixel_snap(unsigned int ppu = 32)
		{
			m_perception.set_scale(ppu * m_pixel_zoom * 2.0f / static_cast<float>(m_screen.x()));
		}

		perception const& view() const noexcept
		{
			return m_perception;
		}
		void frame(double time)
		{
			update(m_perception, time);
		}
		void resize(unsigned int width, unsigned int height)
		{
			m_screen = { static_cast<double>(width), static_cast<double>(height) };
			m_cells = { static_cast<int>(width / gui_cell_width), static_cast<int>(height / gui_cell_height) };
			m_perception.canvas().resize(m_cells);
			pixel_snap();
		}

	public:
		shell()
			: m_pixel_zoom(zoom_default)
			, m_hover(0, 0)
			, m_screen(0, 0)
			, m_cells(0, 0)
		{
			pixel_snap();
		}

	private:
		point2 translate_gui(point2 pixel_coordinates) const
		{
			return (pixel_coordinates / m_screen * m_cells).floor();
		}
		point2 translate_world(point2 pixel_coordinates) const
		{
			vector2 position = pixel_coordinates / m_screen; // (0, 1) screen space

			// (0, 1) -> (-1, +1) and reverse y
			position.move(vector2{ -0.5, -0.5 });
			position.multiply(vector2{ 2.0, -2.0 * m_screen.y() / m_screen.x() }); // account aspect
			
			position /= m_perception.scale();

			return position.floor();
		}

	private:
		perception m_perception;

		// ui transforms
		point2 m_hover;
		vector2 m_screen;
		point2 m_cells;
		int m_pixel_zoom;
	};
}