// name: shell.hpp
// type: c++ header
// auth: is0urce
// desc: input manager facade and application level event dispatcher

#pragma once

#include "key.hpp"

namespace px
{
	class shell
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

	private:
		int hover_x;
		int hover_y;
	};
}