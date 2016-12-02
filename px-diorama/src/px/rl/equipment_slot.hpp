// name: equipment_slot.hpp
// type: c++ header
// desc: enum
// auth: is0urce

#pragma once

namespace px
{
	namespace rl
	{
		enum class equipment_slot : unsigned int
		{
			hand_main,
			hand_off,
			armor_helmet,
			armor_chest,

			min_value = hand_main,
			max_value = armor_chest
		};
	}
}