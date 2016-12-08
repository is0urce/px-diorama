// name: glfw_callback.hpp
// type: c++
// auth: is0urce
// desc: glfw state

#pragma once

// RAII glfw
// setter container for callback functions

#include <GLFW/glfw3.h>

#include <functional>

namespace px
{
	static void glfw_on_resize(GLFWwindow *, int, int);
	class glfw_callback final
	{
	public:
		//typedef void(resize_ptr)(GLFWwindow *, int, int);
		//typedef (*)(GLFWwindow *, int, int) resize_ptr;
		typedef std::function<void(int, int)> resize_fn;
		//typedef std::function<resize_ptr> resize_fn;

	public:
		resize_fn& resize() noexcept
		{
			return m_resize;
		}
		template <typename Callable>
		void on_resize(Callable && fn)
		{
			m_resize = std::forward<Callable>(fn);
			glfwSetWindowSizeCallback(m_window, &glfw_on_resize);
		}

	public:
		glfw_callback(GLFWwindow * window)
			: m_window(window)
		{
			glfwSetWindowUserPointer(window, this);
		}
		~glfw_callback()
		{
		}
	private:
		GLFWwindow * m_window;
		resize_fn m_resize;
	};
	static void glfw_on_resize(GLFWwindow * window, int width, int height)
	{
		auto * cb = reinterpret_cast<px::glfw_callback*>(glfwGetWindowUserPointer(window));
		cb->resize()(width, height);
	}
}