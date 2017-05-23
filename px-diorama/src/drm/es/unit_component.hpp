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

		// useables
		storage		= 's',
		workshop    = 'w',

		// controls
		player		= 'P',
		npc			= 'M',

		// unspecified component indicator used mostly for indicating components that is not serialized
		undefined	= '!'
	};
}