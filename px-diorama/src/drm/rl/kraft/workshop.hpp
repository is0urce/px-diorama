// name: workshop.hpp
// type: c++ header
// auth: is0urce

#pragma once

#include "recipe_category.hpp"

#include <px/common/assert.hpp>
#include <px/rl/loot/item.hpp>

#include <memory>

namespace px {
	namespace rl {

		class workshop
		{
		public:
			static std::shared_ptr<item> make(recipe_category category, std::shared_ptr<item> const& base, std::vector<std::shared_ptr<item>> const& ingredients)
			{
				switch (category)
				{
				case recipe_category::weapon: {
					return make_weapon(base, ingredients);
				}
				default:
					px_assert(0);
					return nullptr;
				}
			}
			static std::shared_ptr<item> make_weapon(std::shared_ptr<item> const& base, std::vector<std::shared_ptr<item>> const& ingredients)
			{
				auto result = std::make_shared<item>();

				px_assert(base);
				if (base) {
					*result = *base;
				}

				return result;
			}

		};
	}
}