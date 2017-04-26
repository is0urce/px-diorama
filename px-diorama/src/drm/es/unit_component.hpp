// name: unit_component.hpp
// type: c++ header
// desc: enum class
// auth: is0urce

#pragma once

#include "unit.hpp"

#include <cstdint>

namespace px {

	// values used by serizlizer, so they are set to some actual values to avoid versions conflicts
	// use hex notation for binary readability
	enum class unit_component : std::uint8_t
	{
		// used mostly for indicating components that is not serialized
		undefined	= 0x4E, // n

		transform	= 0x54, // t
		sprite		= 0x73, // s
		body		= 0x30,
		container	= 0x40,
		storage		= 0x50,
		player		= 0x60
	};

	enum class unit_persistency : std::uint8_t
	{
		serialized = 0,
		permanent,
		temporary
	};
}