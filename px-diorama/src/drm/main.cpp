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

	int application()
	{
		try
		{
			try
			{
				// load config
				auto config = nlohmann::json::parse(std::ifstream("data/config.json"));

				unsigned int screen_width = config["window"]["width"];
				unsigned int screen_height = config["window"]["height"];
				unsigned int vsync = config["window"]["vsync"];

				// create window and context
				glfw_instance instance;
				glfw_window window = glfwCreateWindow(screen_width, screen_height, "press-x-diorama", nullptr, nullptr);
				glfwMakeContextCurrent(window);
				glfwSwapInterval(vsync);
				glewInit();	// initialize extensions loader (need context first)

				// create logic structure
				renderer graphics(screen_width, screen_height);
				bindings<int, key> bindings(config["bindings"]);
				shell game;

				// setup - load data
				for (auto const& texture : config["textures"])
				{
					std::string path = texture["path"];
					std::string atlas = texture["meta"];

					std::vector<unsigned char> image;
					unsigned int w, h;
					auto error = lodepng::decode(image, w, h, path);
					if (error) throw std::runtime_error(std::string("png decoder error in'") + path + "' code#" + std::to_string(error) + std::string(": message=") + std::string(lodepng_error_text(error)));

					graphics.add_texture(w, h, image.data());
					game.add_atlas(nlohmann::json::parse(std::ifstream(atlas))["meta"], true);
				}

				// setup - callbacks
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
				while (window.process())
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
	return px::application();
}