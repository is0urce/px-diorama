// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lodepng.h"

#include "glfw_instance.hpp"

#include "draw/perception.hpp"
#include "draw/renderer.hpp"

#include <px/common/logger.hpp>
#include <px/common/timer.hpp>
#include <px/common/fps_counter.hpp>

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

			unsigned int screen_width = 800;
			unsigned int screen_height = 600;

			// create window and contest
			auto *window = glfwCreateWindow(screen_width, screen_height, "press-x-diorama", nullptr, nullptr);
			glfwMakeContextCurrent(window);
			glewInit();	// initialize extensions wrangler (need context first)

			px::perception data;
			px::renderer graphics(screen_width, screen_height);
			for (int i = 0; i < 5; ++i)
			{
				std::vector<unsigned char> image;
				unsigned int w, h;
				unsigned int error = lodepng::decode(image, w, h, "data/img/px_pug.png");
				if (error) throw std::runtime_error(std::string("decoder error ") + std::to_string(error) + std::string(": ") + std::string(lodepng_error_text(error)));

				graphics.load_texture(w, h, image.data());
				data.add_texture();
			}

			glfwSetKeyCallback(window, key_callback);
			glfwSwapInterval(1); // vsync on

			auto time = []() { return glfwGetTime(); };
			px::timer<decltype(time)> timer(time);
			px::fps_counter<decltype(timer)> fps(&timer);

			// main loop
			while (!glfwWindowShouldClose(window))
			{
				fps.frame();
				graphics.render(data);

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