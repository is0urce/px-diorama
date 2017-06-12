// name: configuration.hpp
// type: c++
// auth: is0urce
// desc: configuration

#pragma once

namespace px {

	class configuration
	{
	public:
		template <typename Document>
		void load(Document const& document)
		{
			screen_width = document["window"]["width"];
			screen_height = document["window"]["height"];
			vsync = document["window"]["vsync"];
			border = document["window"]["border"];
			fullscreen = document["window"]["fullscreen"];
		}

	public:
		unsigned int screen_width;
		unsigned int screen_height;
		int vsync;
		bool border;
		bool fullscreen;
	};
}