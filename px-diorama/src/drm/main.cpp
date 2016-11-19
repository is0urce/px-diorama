// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL\glew.h>
#include <GLFW/glfw3.h>

#include <px/common/logger.hpp>

#include <stdexcept>
#include <string>


class glfw_init
{
public:
	glfw_init()
	{
		glfwSetErrorCallback(&error_callback);
		m_init = glfwInit() != 0;
	}
	~glfw_init()
	{
		if (m_init)
		{
			glfwTerminate();
		}
	}
	bool success() const noexcept
	{
		return m_init;
	}
	static void error_callback(int error, const char* description)
	{
		throw std::runtime_error(std::string("glfw error code = " + std::to_string(error) + " " + std::string(description)));
	}
private:
	bool m_init;
};


int main()
{
	px::logger logger;
	try
	{
		glfw_init init;

		if (!init.success())
		{
			throw std::runtime_error("glfwInit error");
		}
		else
		{
			auto *window = glfwCreateWindow(800, 600, "prss-x-drm", nullptr, nullptr);
			if (window == nullptr) throw std::runtime_error("glfwCreateWindow");
			glfwMakeContextCurrent(window);
			glewInit(); // need context first

			while (!glfwWindowShouldClose(window))
			{
				glClear(GL_COLOR_BUFFER_BIT);
				glClearColor(1, 1, 0, 0);

				glfwSwapBuffers(window);
				glfwPollEvents();
			}
		}
	}
	catch (std::runtime_error &exc)
	{
		logger.write(exc.what());
		return -1;
	}
	catch (...)
	{
		logger.write("unhandled error");
		return -1;
	}

	return 0;
}