// name: unit_persistency.hpp
// type: c++ header
// desc: enum class
// auth: is0urce

#pragma once

#include <cstdint>

namespace px {

	enum class unit_persistency : std::uint8_t
	{
		serialized = 0,
		permanent = 1,
		temporary = 2
	};
}