// name: traverse.hpp
// type: c++ header
// desc: enum
// auth: is0urce

// traverse layers indices enum

#pragma once

namespace px {
	namespace rl {

		enum class traverse : unsigned int
		{
			floor = 0,
			swim = 1,
			fly = 2,
			teleport = 3,

			door = 4, // can be traversed by one who can open doors
			debris = 5, // can be traversed by one who can destroy debris

			not_valid,
			max_value = not_valid
		};
	}
}