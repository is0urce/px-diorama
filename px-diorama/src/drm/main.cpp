// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lodepng.h"

#include <px/common/logger.hpp>

#include "renderer.hpp"

#include <stdexcept>
#include <string>

// RAII glfw init and termination
class glfw_instance
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

static void error_callback(int error, const char* description)
{
	throw std::runtime_error(std::string("glfw error, code #" + std::to_string(error) + " message: " + std::string(description)));
}
static void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) noexcept
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

// application starts here
int main()
{
	try
	{
		try
		{
			glfwSetErrorCallback(&error_callback);
			glfw_instance init;

			// create window and contest
			auto *window = glfwCreateWindow(800, 600, "press-x-diorama", nullptr, nullptr);
			glfwSetKeyCallback(window, key_callback);
			glfwMakeContextCurrent(window);
			glewInit();	// initialize extensions wrangler (need context first)
			glfwSwapInterval(1); // vsync on

			px::renderer graphics(800, 600);
			std::vector<unsigned char> image;
			unsigned int width, height;
			unsigned int error = lodepng::decode(image, width, height, "data/img/dtc.png");
			if (error) throw std::runtime_error(std::string("decoder error ") + std::to_string(error) + std::string(": ") + std::string(lodepng_error_text(error)));
			graphics.load_texture(32, 23, image.data());

			// main loop
			while (!glfwWindowShouldClose(window))
			{
				graphics.render();
				
				// io
				glfwSwapBuffers(window);
				glfwPollEvents();
			}

			glfwDestroyWindow(window);
		}
		catch (std::runtime_error &exc)
		{
			throw std::move(exc);
		}
		catch (...)
		{
			throw std::runtime_error("unhandled exception");
		}
	}
	catch (std::runtime_error &exc)
	{
		px::logger logger;
		logger.write(exc.what());
		return -1;
	}
	return 0;
}