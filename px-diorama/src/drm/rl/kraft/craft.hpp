// name: craft.hpp
// type: c++ header
// auth: is0urce

#pragma once

#include <px/rl/loot/item.hpp>

#include <memory>

namespace px {
	namespace rl {

		class craft
		{
		public:
			static std::shared_ptr<item> make_weapon(item const& base, std::vector<item const*> const& ingredients)
			{
				auto result = std::make_shared<item>();

				*result = base;

				int ore_power = 0;
				for (item const* ore : ingredients) {
					if (ore) {
						ore_power += ore->value<rl::effect::ore_power>();
					}
				}

				result->add(item::enhancement_type::integer(effect::weapon_damage, ore_power));

				return result;
			}
		};
	}
}