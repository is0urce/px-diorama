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

		quick_save = 105,
		quick_load = 109,

		navigate_next,
		navigate_previous,

		panel_inventory,
		panel_character,
		panel_map,
		panel_journal,

		command_ok,
		command_cancel,
	};
}