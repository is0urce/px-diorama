// name: traverse.hpp
// type: c++ header
// desc: enum
// auth: is0urce

// traverse layers indices enum

#pragma once

namespace px
{
	namespace rl
	{
		enum class traverse : unsigned int
		{
			lava,
			floor = lava,
			swim,
			fly,
			teleport,
			door, // can be traversed by one who can open doors
			debris, // can be traversed by one who can destroy debris

			not_valid,
			max_value = not_valid
		};
	}
}