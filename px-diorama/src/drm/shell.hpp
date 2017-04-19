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
		void text(unsigned int /*codepoint*/)
		{
		}
		void hover(int x, int y)
		{
			m_hover = { x, y };

			target(translate_world(m_hover));
		}
		void click(int button)
		{
			bool processed = ui().click(translate_gui(m_hover), button);

			if (!processed)
			{
				activate(0);
			}
		}
		void scroll(double vertical, double horisontal)
		{
			m_perception.zoom(static_cast<float>(vertical + horisontal) * 0.1f);
		}

		template <typename Document>
		void add_atlas(Document && atlas, bool reverse_y)
		{
			m_perception.add_texture();
			add_sprite_atlas(atlas, reverse_y);
		}

		perception const& view() const noexcept
		{
			return m_perception;
		}
		void frame(double /*time*/)
		{
			// clear sprite batches
			m_perception.clear_batches();

			// clear ui grid
			m_perception.canvas().cls();
			layout_ui(m_perception.canvas().range());

			write(m_perception);
		}
		void resize(unsigned int width, unsigned int height)
		{
			m_screen_width = width;
			m_screen_height = height;
			m_perception.canvas().resize(width / gui_cell_width, height / gui_cell_height);
		}

	public:
		shell()
		{
			m_perception.zoom(-0.95f);
		}

	private:
		point2 translate_gui(point2 pixel_coordinates) const
		{
			return pixel_coordinates / point2(gui_cell_width, gui_cell_height);
		}
		point2 translate_world(point2 pixel_coordinates) const
		{
			return pixel_coordinates;
		}

	private:
		perception m_perception;

		// ui transforms
		point2 m_hover;
		unsigned int m_screen_width;
		unsigned int m_screen_height;
		unsigned int m_ui_width;
		unsigned int m_ui_height;
	};
}