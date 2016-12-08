// name: glfw_time.cpp
// type: c++
// auth: is0urce
// desc: glfw lambda

#pragma once

// glfw time querry operator

#include <GLFW/glfw3.h>

namespace px
{
	class glfw_time final
	{
	public:
		double operator()() const
		{
			return glfwGetTime();
		}
	};
}