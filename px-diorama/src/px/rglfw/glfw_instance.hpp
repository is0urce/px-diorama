// name: glfw_instance.hpp
// type: c++
// auth: is0urce
// desc: class

#pragma once

// RAII glfw init and termination
// glfw state

#include <GLFW/glfw3.h>

namespace px
{
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
		glfw_instance(GLFWerrorfun error_callback)
			: m_init(false)
		{
			glfwSetErrorCallback(error_callback);
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
}