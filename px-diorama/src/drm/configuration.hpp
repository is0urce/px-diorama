// name: configuration.hpp
// type: c++
// auth: is0urce
// desc: configuration

#pragma once

#include <stdexcept>

namespace px {

	class configuration
	{
	public:
		template <typename Document>
		void load(Document const& document)
		{
			try
			{
				screen_width = document["window"]["width"];
				screen_height = document["window"]["height"];
				vsync = document["window"]["vsync"];
				border = document["window"]["border"];
				fullscreen = document["window"]["fullscreen"];
			}
			catch (std::exception & exc)
			{
				throw std::runtime_error("error while loading configuration, what=" + std::string(exc.what()));
			}
		}

		template <typename Document>
		static configuration from_document(Document && document)
		{
			configuration result;
			result.load(std::forward<Document>(document));
			return result;
		}

	public:
		unsigned int screen_width;
		unsigned int screen_height;
		int vsync;
		bool border;
		bool fullscreen;
	};
}