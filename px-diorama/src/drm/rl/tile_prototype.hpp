// name: tile_prototype.hpp
// type: c++
// auth: is0urce
// desc: struct

#pragma once

#include <cstdint>
#include <string>

namespace px {

	template <typename Mass>
	struct tile_prototype
	{
		uint32_t id;
		std::string background;
		Mass mass;
	};
}
