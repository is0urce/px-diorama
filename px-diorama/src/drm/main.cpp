// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "lodepng.h"
#include <json.hpp>

#include "glfw_instance.hpp"
#include "glfw_time.hpp"
#include "glfw_window.hpp"

#include "shell.hpp"
#include "key.hpp"
#include "perception.hpp"
#include "draw/renderer.hpp"

#include <px/common/logger.hpp>
#include <px/common/timer.hpp>
#include <px/common/fps_counter.hpp>
#include <px/common/bindings.hpp>

#include <algorithm>
#include <stdexcept>
#include <string>

void error_callback(int error, const char* description);
void key_callback(GLFWwindow * window, int key, int /* scancode */, int action, int /* mods */);
void text_callback(GLFWwindow * window, unsigned int codepoint);
void click_callback(GLFWwindow * window, int button, int action, int /* mods */);
void hover_callback(GLFWwindow * window, double x, double y);
void scroll_callback(GLFWwindow * window, double x, double y);

px::bindings<int, px::key> g_bindings;

int main() // application starts here
{
	try
	{
		try
		{
			// load config
			auto doc = nlohmann::json::parse(std::ifstream("data/config.json"));

			unsigned int screen_width = doc["window"]["width"];
			unsigned int screen_height = doc["window"]["height"];
			unsigned int vsync = doc["window"]["vsync"];

			// create window and context
			glfwSetErrorCallback(&error_callback);
			glfw_instance instance;
			glfw_window window = glfwCreateWindow(screen_width, screen_height, "press-x-diorama", nullptr, nullptr);
			glfwMakeContextCurrent(window);
			glfwSwapInterval(vsync);
			glewInit();	// initialize extensions wrangler (need context first)

			// create and populate internal states
			px::renderer graphics(screen_width, screen_height);
			px::shell game;

			for (auto const& binding : doc["bindings"])
			{
				std::underlying_type<px::key>::type action_index = binding["action"];
				for (auto const& key : binding["keys"])
				{
					g_bindings.bind(key, static_cast<px::key>(action_index));
				}
			}
			for (auto const& texture : doc["textures"])
			{
				std::vector<unsigned char> image;
				unsigned int w, h;
				std::string name = texture["name"];
				auto error = lodepng::decode(image, w, h, name);
				if (error) throw std::runtime_error(std::string("png decoder error in'") + name + "' code#" + std::to_string(error) + std::string(": message=") + std::string(lodepng_error_text(error)));

				graphics.load_texture(w, h, image.data());
				game.load_texture(texture["meta"]);
			}
			game.start();

			glfwSetWindowUserPointer(window, &game);
			glfwSetKeyCallback(window, key_callback);
			glfwSetCharCallback(window, text_callback);
			glfwSetMouseButtonCallback(window, click_callback);
			glfwSetCursorPosCallback(window, hover_callback);
			glfwSetScrollCallback(window, scroll_callback);

			px::timer<glfw_time> time;

			// main loop
			while (window)
			{
				game.frame(time);
				graphics.render(game.view());

				window.process();
			}
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

void error_callback(int error, const char* description)
{
	throw std::runtime_error(std::string("glfw error, code #" + std::to_string(error) + " message: " + std::string(description)));
}
void key_callback(GLFWwindow * window, int key, int /* scancode */, int action, int /* mods */)
{
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		reinterpret_cast<px::shell*>(glfwGetWindowUserPointer(window))->press(g_bindings.select(key, px::key::not_valid));
	}
}
void text_callback(GLFWwindow * window, unsigned int codepoint)
{
	reinterpret_cast<px::shell*>(glfwGetWindowUserPointer(window))->text(codepoint);
}
void click_callback(GLFWwindow * window, int button, int action, int /* mods */)
{
	if (action == GLFW_PRESS)
	{
		reinterpret_cast<px::shell*>(glfwGetWindowUserPointer(window))->click(button);
	}
}
void hover_callback(GLFWwindow * window, double x, double y)
{
	reinterpret_cast<px::shell*>(glfwGetWindowUserPointer(window))->hover(static_cast<int>(x), static_cast<int>(y));
}
void scroll_callback(GLFWwindow * window, double x, double y)
{
	reinterpret_cast<px::shell*>(glfwGetWindowUserPointer(window))->scroll(y, x);
}