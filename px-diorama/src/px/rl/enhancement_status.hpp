// name: enhancement_status.hpp
// type: c++ header
// desc: enum
// auth: is0urce

// status types

#pragma once

namespace px {
	namespace rl {

		enum class enhancement_status : unsigned char
		{
			neutral = 0,	// base stat
			positive,		// plus stat
			negative,		// minus stat
			modified,		// both positive and negative, or unspecified
			descriptive,	// informative
			special,		// out of classification

			not_valid,
			max_value = not_valid
		};
	}
}