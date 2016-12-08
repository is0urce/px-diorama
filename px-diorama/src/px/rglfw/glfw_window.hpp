// name: glfw_window.cpp
// type: c++
// auth: is0urce
// desc: glfw state

#pragma once

// RAII glfw window

#include <GLFW/glfw3.h>
namespace px
{
	class glfw_window final
	{
	public:
		operator bool() const
		{
			return m_window && !glfwWindowShouldClose(m_window);
		}
		operator GLFWwindow * () const noexcept
		{
			return m_window;
		}
		bool process()
		{
			glfwSwapBuffers(m_window);
			glfwPollEvents();
			return !glfwWindowShouldClose(m_window);
		}

	public:
		glfw_window() noexcept
			: m_window(nullptr)
		{
		}
		glfw_window(GLFWwindow * window) noexcept
			: m_window(window)
		{
		}
		~glfw_window()
		{
			if (m_window)
			{
				glfwDestroyWindow(m_window);
			}
		}
	private:
		GLFWwindow * m_window;
	};
}