// name: glfw_instance.hpp
// type: c++
// auth: is0urce
// desc: class

#pragma once

// RAII glfw init and termination
// glfw state

#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

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
		glfw_instance(GLFWerrorfun error_callback)
			: m_init(false)
		{
			if (error_callback)
			{
				glfwSetErrorCallback(error_callback);
			}
			m_init = glfwInit() != 0;
		}
		glfw_instance()
			: glfw_instance(&error_callback)
		{
		}
		~glfw_instance()
		{
			if (m_init)
			{
				glfwTerminate();
			}
		}

	private:
		static void error_callback(int error, const char* description)
		{
			throw std::runtime_error(std::string("glfw error, code #" + std::to_string(error) + " message: " + std::string(description)));
		}

	private:
		bool m_init;
	};
}