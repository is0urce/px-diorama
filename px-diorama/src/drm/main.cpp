// name: main.cpp
// type: c++
// auth: is0urce
// desc: entry point

#define GLEW_STATIC
#include <GL/glew.h>
#include <px/rglfw/rglfw.hpp>

#include "configuration.hpp"
#include "settings.hpp"
#include "shell.hpp"
#include "draw/renderer.hpp"
#include "key.hpp"

#include <px/common/logger.hpp>
#include <px/common/timer.hpp>
#include <px/common/bindings.hpp>

#include <lodepng.h>
#include <json.hpp>
#include <date/date.h>

#include <stdexcept>
#include <string>

namespace px {

	auto load_document(std::string const& document_depot)
	{
		std::ifstream file(document_depot);
		if (!file.is_open()) throw std::runtime_error("error opening file path=" + document_depot);
		return nlohmann::json::parse(file);
	}
	glfw_window create_window(configuration & config)
	{
		auto monitor = glfwGetPrimaryMonitor();
		auto mode = glfwGetVideoMode(monitor);

		glfwWindowHint(GLFW_DECORATED, config.border ? 1 : 0); // border
		if (config.fullscreen) {
			glfwWindowHint(GLFW_RED_BITS, mode->redBits);
			glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
			glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
			glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
			config.screen_width = mode->width;
			config.screen_height = mode->height;
		}

		glfw_window window = glfwCreateWindow(config.screen_width, config.screen_height, "press-x-diorama", config.fullscreen ? monitor : nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(config.vsync);

		return window;
	}
	void load_textures(renderer & graphics, std::string const& atlas_path)
	{
		std::ifstream file(atlas_path);
		if (!file.is_open()) throw std::runtime_error("error opening file path=" + std::string(atlas_path));
		auto textures = nlohmann::json::parse(file);

		for (auto const& texture : textures["textures"]) {
			std::string path = texture["texture"];

			std::vector<unsigned char> image;
			unsigned int texture_width;
			unsigned int texture_height;
			auto error = lodepng::decode(image, texture_width, texture_height, path);
			if (error) throw std::runtime_error(std::string("png decoder error in'") + path + "' code#" + std::to_string(error) + std::string(": message=") + std::string(lodepng_error_text(error)));

			graphics.add_texture(texture_width, texture_height, image.data());
		}
	}

	int run_application()
	{
		try
		{
			try
			{
				// configuration
				bindings<int, key> binds;
				configuration config;		
				try
				{
					binds.load(load_document(keybindings_path)["bindings"]);
				}
				catch (std::exception & exc)
				{
					throw std::runtime_error("error while loading bindings in=" + std::string(keybindings_path) + " what=" + std::string(exc.what()));
				}
				try
				{
					config.load(load_document(configuration_path));
				}
				catch (std::exception & exc)
				{
					throw std::runtime_error("error while loading configuration in=" + std::string(configuration_path) + " what=" + std::string(exc.what()));
				}

				// graphics setup
				// order is important, window context first, gl extensions second, renderer state last
				glfw_instance instance;
				glfw_window window = create_window(config);
				glewInit();	// OpenGL extensions

				renderer graphics(config.screen_width, config.screen_height);
				load_textures(graphics, textureatlas_path);

				shell game(config.screen_width, config.screen_height);

				// setup callback procedures for window message handling
				glfw_callback callback(window);
				callback.on_resize([&](auto /* window */, int widht, int height) {
					config.screen_width = widht;
					config.screen_height = height;

					graphics.resize(config.screen_width, config.screen_height);
					game.resize(config.screen_width, config.screen_height);
				});
				callback.on_key([&](auto /* window */, int os_key, int /* scancode */, int action, int /* mods */) {
					if (action == GLFW_PRESS || action == GLFW_REPEAT) game.press(binds.select(os_key, key::not_valid));
				});
				callback.on_text([&](auto /* window */, unsigned int codepoint) {
					game.text(codepoint);
				});
				callback.on_click([&](auto /* window */, int button, int action, int /* mods */) {
					if (action == GLFW_PRESS) game.click(button);
				});
				callback.on_hover([&](auto /* window */, double x, double y) {
					game.hover(static_cast<int>(x), static_cast<int>(y));
				});
				callback.on_scroll([&](auto /* window */, double horisontal, double vertical) {
					game.scroll(horisontal, vertical);
				});

				// main loop
				timer<glfw_time> time;
				while (window.process() && game.running()) {
					game.frame(time);
					graphics.render(game.view());
				}
			}
			catch (std::exception & exc)
			{
				throw std::move(exc);
			}
			catch (...)
			{
				throw std::exception("unhandled exception");
			}
		}
		catch (std::exception & exc)
		{
			auto now = std::chrono::system_clock::now();
			auto today = date::floor<date::days>(now);

			std::stringstream ss;
			ss << date::year_month_day(today) << " " << date::make_time(now - today) << " UTC, exception thrown, what=\"" << exc.what() << "\"";

			logger logger;
			logger.write(ss.str());
			return -1;
		}
		return 0;
	}
}

int main()
{
	return px::run_application();
}