// name: glfw_window.cpp
// type: c++
// auth: is0urce
// desc: glfw state

#pragma once

// RAII glfw window

#include <GLFW/glfw3.h>

#include <px/common/assert.hpp>

namespace px
{
	class glfw_window final
	{
	public:
		GLFWwindow * window() const noexcept
		{
			return m_window;
		}
		void make_current()
		{
			px_assert(m_window);

			glfwMakeContextCurrent(m_window);
		}
		bool process()
		{
			px_assert(m_window);

			glfwSwapBuffers(m_window);
			glfwPollEvents();
			return !glfwWindowShouldClose(m_window);
		}
		operator bool() const
		{
			return m_window && !glfwWindowShouldClose(m_window);
		}
		operator GLFWwindow * () const noexcept
		{
			return m_window;
		}
		void swap(glfw_window & that) noexcept
		{
			std::swap(m_window, that.m_window);
		}
		void close()
		{
			if (m_window) {
				glfwDestroyWindow(m_window);
				m_window = nullptr;
			}
		}

	public:
		~glfw_window()
		{
			close();
		}
		glfw_window() noexcept
			: m_window(nullptr)
		{
		}
		glfw_window(GLFWwindow * window) noexcept
			: m_window(window)
		{
		}
		glfw_window(glfw_window && that)
			: glfw_window()
		{
			swap(that);
		}
		glfw_window(glfw_window const&) = delete;
		glfw_window & operator=(glfw_window const&) = delete;
		glfw_window & operator=(GLFWwindow * window)
		{
			if (m_window != window) {
				close();
				m_window = window;
			}
			return *this;
		}

	private:
		GLFWwindow * m_window;
	};
}