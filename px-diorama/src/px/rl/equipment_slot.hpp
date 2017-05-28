// name: equipment_slot.hpp
// type: c++ header
// desc: enum
// auth: is0urce

#pragma once

#include <cstdint>

namespace px {
	namespace rl {

		enum class equipment_slot : uint32_t
		{
			hand_main,
			hand_off,
			armor_helmet,
			armor_chest,
			accessory_bell,

			min_value = hand_main,
			max_value = accessory_bell
		};
	}
}