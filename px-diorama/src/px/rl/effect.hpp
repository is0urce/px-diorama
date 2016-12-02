// name: effect.hpp
// type: c++ header
// desc: enum
// auth: is0urce

#pragma once

namespace px
{
	namespace rl
	{
		enum class effect : unsigned int
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