// name: glfw_instance.cpp
// type: c++
// auth: is0urce
// desc: glfw state

#pragma once

// RAII glfw init and termination

#include <GLFW/glfw3.h>

class glfw_instance final
{
public:
	bool success() const noexcept
	{
		return m_init;
	}

public:
	glfw_instance()
		: m_init(false)
	{
		m_init = glfwInit() != 0;
	}
	~glfw_instance()
	{
		if (m_init)
		{
			glfwTerminate();
		}
	}
private:
	bool m_init;
};