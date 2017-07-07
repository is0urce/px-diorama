// name: unit_component.hpp
// type: c++ header
// desc: enum class
// auth: is0urce

#pragma once

#include <cstdint>

namespace px {

	// values used by serizlizer, so they are set to some actual values to avoid versions conflicts
	// use hex notation for binary readability
	enum class unit_component : std::uint32_t
	{
		// core data
		transform	= 'T',
		sprite		= 'S',
		animator	= 'A',
		body		= 'B',
		container	= 'I',
		character	= 'C',
		light		= 'L',
		sound		= 'W',

		// useables
		storage		= 's',
		workshop	= 'w',
		deposit		= 'd',

		// controls
		player		= 'P',
		npc			= 'M',
		undefined	= '!' // unspecified component, used mostly for indicating components that is not serialized
	};
}