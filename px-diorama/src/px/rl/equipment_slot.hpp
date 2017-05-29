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
			weapon_main,
			weapon_off,

			armor_helmet,
			armor_chest,

			accessory_bell,

			min_value = weapon_main,
			max_value = accessory_bell
		};
	}
}