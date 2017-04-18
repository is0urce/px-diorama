// name: recipe.hpp
// type: c++ header
// auth: is0urce

#pragma once

#include <string>

namespace px {

	enum class recipe_type : unsigned int
	{
		weapon, armor
	};

	struct recipe
	{
		std::string name;
		recipe_type slot;
		unsigned int count;
	};
}
