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
			neutral = 0,	// base stat (white)
			positive,		// plus stat (green/blue)
			negative,		// minus stat (red)
			modified,		// both positive and negative, or unspecified (blue/red)
			general,		// (bold)
			descriptive,	// informative (italic)
			special,		// out of classification

			not_valid,
			max_value = not_valid
		};
	}
}