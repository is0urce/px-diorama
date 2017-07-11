// name: effect.hpp
// type: c++ header
// desc: enum
// auth: is0urce

// types of available effect for buffs / items enhancement

#pragma once

#include <cstdint>

namespace px {
	namespace rl {

		enum class effect : uint32_t
		{
			no_effect,

			// main character props

			str_bonus,
			dex_bonus,
			int_bonus,

			// secondary character attributes

			resistance,			// bonus armor
			dodge,
			accuracy,
			critical,			// chance of double damage
			hp_bonus,
			hp_regen,
			mp_bonus,
			mp_regen,

			// buff mechanics

			periodic_damage,	// damage over time effects
			thorns_damage,

			// item specific props

			equipment,		// subtype is slot
			value,			// appraised merchant cost
			weight,

			weapon_damage,
			armor_resistance,

			ore_power,
			reagent_power,
			essence,		// unique 'id hash' of an item material

			// special

			not_valid,
			max_value = not_valid
		};
	}
}