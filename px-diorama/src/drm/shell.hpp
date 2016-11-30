// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key.hpp"
#include "perception.hpp"

namespace px
{
	class shell final
	{
	public:
		void press(key /*action*/)
		{

		}
		void text(unsigned int /*codepoint*/)
		{

		}
		void click(int /*button*/)
		{

		}
		void hover(int x, int y)
		{
			hover_x = x;
			hover_y = y;
		}
		void scroll(double /*vertical*/, double /*horisontal*/)
		{

		}
		void frame(double /*time*/)
		{

		}
		void load_texture()
		{
			m_perception.add_texture();
		}

		perception const& view() const noexcept
		{
			return m_perception;
		}

	private:
		perception m_perception;
		int hover_x;
		int hover_y;
	};
}