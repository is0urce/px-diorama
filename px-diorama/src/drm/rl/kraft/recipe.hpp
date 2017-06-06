// name: recipe.hpp
// type: c++ header
// auth: is0urce

#pragma once

#include "ingredient.hpp"
#include "recipe_category.hpp"

#include <string>

namespace px {
	namespace rl {

		struct recipe
		{
			std::string		name;
			recipe_category	category;	// sorting

			ingredient		ingredient_variation;	// reagent type
			unsigned int	ingredient_count;		// number of reagents required
		};
	}
}