// name: craft.hpp
// type: c++ header
// auth: is0urce

#pragma once

#include "recipe.hpp"

#include <px/common/assert.hpp>
#include <px/rl/loot/item.hpp>

#include <memory>
#include <set>
#include <random>

namespace px {
	namespace rl {

		class crafting
		{
		public:
			static int essence_percent(std::set<int> const& essences)
			{
				return (essences.size() > 1) ? std::accumulate(std::cbegin(essences), std::cend(essences), 1, [&](auto l, auto r) { return l * r; }) % 100 + 50 : 125;
			}
			static std::shared_ptr<item> make(recipe const& recipe, std::vector<item const*> const& ingredients)
			{
				switch (recipe.category)
				{
				case recipe_category::weapon: {
					return make_weapon(recipe, ingredients);
					break;
				}
				case recipe_category::armor: {
					return make_weapon(recipe, ingredients);
					break;
				}
				case recipe_category::potion: {
					return make_weapon(recipe, ingredients);
					break;
				}
				default:
					px_assert(0);
					break;
				}
				return nullptr;
			}
			static std::shared_ptr<item> make_weapon(recipe const& recipe, std::vector<item const*> const& ingredients)
			{
				auto result = std::make_shared<item>(recipe.base_item);

				int ore_power = 0;
				std::set<int> essences;

				for (item const* ingredient : ingredients) {
					if (ingredient) {
						ore_power += ingredient->value<effect::ore_power>();

						essences.insert(ingredient->value<effect::essence>());
					}
				}

				double power = static_cast<double>(ore_power);
				power *= essence_percent(essences) * 0.01;
				power *= (std::rand() % 100 < 5) ? 1.2 : 1.0;

				int weapon_damage = static_cast<int>(std::floor(power));

				result->add(item::enhancement_type::integer(effect::weapon_damage, weapon_damage));

				return result;
			}
			static std::shared_ptr<item> make_armor(recipe const& recipe, std::vector<item const*> const& /*ingredients*/)
			{
				auto result = std::make_shared<item>(recipe.base_item);

				return result;
			}
			static std::shared_ptr<item> make_potion(recipe const& recipe, std::vector<item const*> const& /*ingredients*/)
			{
				auto result = std::make_shared<item>(recipe.base_item);

				*result = recipe.base_item;

				return result;
			}
		};
	}
}