// name: effect.hpp
// type: c++ header
// desc: enum
// auth: is0urce

#pragma once

#include <cstdint>

namespace px {
	namespace rl {

		enum class effect : uint32_t
		{
			no_effect,

			str_bonus,
			dex_bonus,
			int_bonus,

			// item specific props

			equipment,
			weight,
			value,

			weapon_damage,
			armor_resist,

			ore_power,

			not_valid,
			max_value = not_valid
		};
	}
}