// name: unit_component.hpp
// type: c++ header
// desc: enum class
// auth: is0urce

#pragma once

#include <cstdint>

namespace px {

	// values used by serizlizer, so they are set to some actual values to avoid versions conflicts
	// use hex notation for binary readability
	enum class unit_component : std::uint8_t
	{
		// core data
		transform	= 'T',
		sprite		= 'S',
		body		= 'B',
		container	= 'C',

		// useables
		storage		= 's',
		workshop    = 'w',

		// controls
		player		= '@',

		// unspecified component indicator used mostly for indicating components that is not serialized
		undefined	= '!' // n
	};
}