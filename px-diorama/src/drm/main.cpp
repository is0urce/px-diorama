// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lodepng.h"

#include "glfw_instance.hpp"
#include "renderer.hpp"
#include <px/common/logger.hpp>

#include <stdexcept>
#include <string>

static void error_callback(int error, const char* description)
{
	throw std::runtime_error(std::string("glfw error, code #" + std::to_string(error) + " message: " + std::string(description)));
}
static void key_callback(GLFWwindow* window, int key, int /* scancode */, int action, int /* mods */) noexcept
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() // application starts here
{
	try
	{
		try
		{
			glfwSetErrorCallback(&error_callback);
			glfw_instance instance;

			// create window and contest
			auto *window = glfwCreateWindow(800, 600, "press-x-diorama", nullptr, nullptr);
			glfwMakeContextCurrent(window);
			glewInit();	// initialize extensions wrangler (need context first)

			px::renderer graphics(800, 600);
			for (int i = 0; i < 10; ++i)
			{
				std::vector<unsigned char> image;
				unsigned int width, height;
				unsigned int error = lodepng::decode(image, width, height, "data/img/dtc.png");
				if (error) throw std::runtime_error(std::string("decoder error ") + std::to_string(error) + std::string(": ") + std::string(lodepng_error_text(error)));
				graphics.load_texture(width, height, image.data());
			}

			glfwSetKeyCallback(window, key_callback);
			glfwSwapInterval(1); // vsync on

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