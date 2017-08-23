// name: key_action.hpp
// type: c++ header
// desc: system virtual key enum
// auth: is0urce

#pragma once

// virtual keys used by press-x

namespace px {

	enum class key : unsigned int
	{
		not_valid = 0,

		move_southwest = 1,
		move_south = 2,
		move_southeast = 3,
		move_west = 4,
		move_none = 5,
		move_east = 6,
		move_northwest = 7,
		move_north = 8,
		move_northeast = 9,

		action0 = 10,
		action1 = 11,
		action2 = 12,
		action3 = 13,
		action4 = 14,
		action5 = 15,
		action6 = 16,
		action7 = 17,
		action8 = 18,
		action9 = 19,

		action_use = 20,
		action_alt = 21,

		command_ok			= 50,	// enter
		command_cancel		= 51,	// escape
		command_backspace	= 52,	// backspace

		navigate_next		= 60,
		navigate_previous	= 61,

		quick_save = 105,
		quick_load = 109,

		panel_character = 500,
		panel_craft = 501,
		panel_inventory = 502,
		panel_journal = 503,
		panel_map = 504,
		panel_options = 505
	};
}