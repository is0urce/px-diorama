// name: unit_component.hpp
// type: c++ header
// desc: enum class
// auth: is0urce

#pragma once

#include "unit.hpp"

namespace px {

	// values used by serizlizer, so they are set to some actual values to avoid versions conflicts
	enum class unit_component : unsigned int
	{
		// used mostly for indicating components that is not serialized
		undefined = 0xFF,

		transform = 0x10,
		sprite = 0x20,
		body = 0x30,
		container = 0x40,
		storage = 0x50,
		player = 0x60
	};
}