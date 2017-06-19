// name: surface.hpp
// type: c++
// auth: is0urce
// desc: room struct

#pragma once

#include <px/common/rectangle.hpp>
#include <px/fn/surface.hpp>

namespace px {
	namespace fn {

		struct room
		{
			rectangle bounds;
			surface surface;
		};

	}
}