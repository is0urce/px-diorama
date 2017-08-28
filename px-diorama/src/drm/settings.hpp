// name: settings.hpp
// type: c++
// auth: is0urce
// desc: constants

#pragma once

namespace px {

	namespace {

		// editor

		const bool editor_mode = true;

		// config

		char const* const configuration_path = "data/config.json";
		char const* const keybindings_path = "data/controls.json";
		char const* const textureatlas_path = "data/textures.json";
		char const* const tiles_path = "data/tiles.json";

		char const* const blueprint_directory = "data/blueprints/";
		char const* const blueprint_extension = ".dat";
		char const* const terrain_directory = "data/terrain/";
		char const* const terrain_extension = ".dat";

		char const* const save_directory = "data/save";
		char const* const save_extension = ".sav";
		char const* const current_repository = "current";
		char const* const quicksave_repository = "quicksave";
		char const* const base_repository = "base";

		// fonts

		char const* const grid_font_path = "data/fonts/DejaVuSansMono.ttf";
		char const* const popup_font_path = "data/fonts/DejaVuSansMono.ttf";
		const unsigned int grid_font_size = 18;	// size of font
		const unsigned int grid_font_atlas = 512;	// internal size of font atlas
		const unsigned int popup_font_size = 18;
		const unsigned int popup_font_atlas = 512;

		// world

		const size_t cell_width = 10;
		const size_t cell_height = cell_width;
	}
}