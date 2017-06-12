// name: settings.hpp
// type: c++
// auth: is0urce
// desc: constants

#pragma once

namespace px {

	namespace {

		char const* configuration_path = "data/config.json";
		char const* keybindings_path = "data/controls.json";
		char const* textureatlas_path = "data/textures.json";
		char const* tiles_path = "data/tiles.json";

		// fonts
		char const* grid_font_path = "data/fonts/DejaVuSansMono.ttf";
		char const* popup_font_path = "data/fonts/DejaVuSansMono.ttf";
		unsigned int grid_font_size = 18;	// size of font
		unsigned int grid_font_atlas = 512;	// internal size of font atlas
		unsigned int popup_font_size = 18;
		unsigned int popup_font_atlas = 512;
	}
}