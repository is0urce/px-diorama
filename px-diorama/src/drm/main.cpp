// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL/glew.h>
#include <px/rglfw/rglfw.hpp>

#include <lodepng.h>
#include <json.hpp>

#include "shell.hpp"
#include "draw/renderer.hpp"

#include <px/common/logger.hpp>
#include <px/common/timer.hpp>
#include <px/common/bindings.hpp>

#include <stdexcept>
#include <string>

namespace px {

	int run_application()
	{
		try
		{
			try
			{
				// load configuration
				auto config = nlohmann::json::parse(std::ifstream("data/config.json"));

				unsigned int screen_width = config["window"]["width"];
				unsigned int screen_height = config["window"]["height"];
				unsigned int vsync = config["window"]["vsync"];
				bool border = config["window"]["border"];
				bool fullscreen = config["window"]["fullscreen"];

				// create window and context
				glfw_instance instance;

				glfwWindowHint(GLFW_DECORATED, border ? 1 : 0); // border

				auto monitor = glfwGetPrimaryMonitor();
				auto mode = glfwGetVideoMode(monitor);
				if (fullscreen)
				{
					glfwWindowHint(GLFW_RED_BITS, mode->redBits);
					glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
					glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
					glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
					screen_width = mode->width;
					screen_height = mode->height;
				}
				else
				{
					monitor = nullptr;
				}

				glfw_window window = glfwCreateWindow(screen_width, screen_height, "press-x-diorama", monitor, nullptr);
				glfwMakeContextCurrent(window);
				glfwSwapInterval(vsync);
				glewInit();	// initialize extensions loader (need context first)

				// create game logic structures
				renderer graphics(screen_width, screen_height);
				bindings<int, key> bindings(nlohmann::json::parse(std::ifstream("data/controls.json"))["bindings"]);
				shell game;

				// load data from configuration settings
				auto textures = nlohmann::json::parse(std::ifstream("data/textures.json"));
				for (auto const& texture : textures["textures"])
				{
					std::string path = texture["path"];
					std::string atlas = texture["meta"];

					std::vector<unsigned char> image;
					unsigned int w, h;
					auto error = lodepng::decode(image, w, h, path);
					if (error) throw std::runtime_error(std::string("png decoder error in'") + path + "' code#" + std::to_string(error) + std::string(": message=") + std::string(lodepng_error_text(error)));

					graphics.add_texture(w, h, image.data());
					auto meta = nlohmann::json::parse(std::ifstream(atlas));
					game.add_atlas(meta["meta"], true);
				}

				// setup callback procedures for window message handling
				glfw_callback callback(window);
				callback.on_resize([&](auto * /* window */, int widht, int height) {
					screen_width = widht;
					screen_height = height;

					graphics.resize(screen_width, screen_height);
					game.resize(screen_width, screen_height);
				});
				callback.on_key([&](auto * /* window */, int key, int /* scancode */, int action, int /* mods */) {
					if (action == GLFW_PRESS || action == GLFW_REPEAT) game.press(bindings.select(key, key::not_valid));
				});
				callback.on_text([&](auto * /* window */, unsigned int codepoint) {
					game.text(codepoint);
				});
				callback.on_click([&](auto * /* window */, int button, int action, int /* mods */) {
					if (action == GLFW_PRESS) game.click(button);
				});
				callback.on_hover([&](auto * /* window */, double x, double y) {
					game.hover(static_cast<int>(x), static_cast<int>(y));
				});
				callback.on_scroll([&](auto * /* window */, double horisontal, double vertical) {
					game.scroll(vertical, horisontal);
				});

				// setup - ui canvas
				game.resize(screen_width, screen_height);

				// start
				game.start();

				// main loop
				timer<glfw_time> time;
				while (window.process() && game.running())
				{
					game.frame(time);
					graphics.render(game.view());
				}
			}
			catch (std::runtime_error & exc)
			{
				throw std::move(exc);
			}
			catch (...)
			{
				throw std::runtime_error("unhandled exception");
			}
		}
		catch (std::runtime_error & exc)
		{
			logger logger;
			logger.write(exc.what());
			return -1;
		}
		return 0;
	}
}

int main()
{
	return px::run_application();
}