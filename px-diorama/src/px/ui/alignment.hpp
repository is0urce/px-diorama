// name: alignment.hpp
// type: c++
// desc: struct declaration
// auth: is0urce

// pod struct for alignment values

#pragma once

#include <px/common/point.hpp>
#include <px/common/vector.hpp>
#include <px/common/rectangle.hpp>

namespace px
{
	namespace ui
	{
		struct alignment
		{
		public:
			vector2 anchor_percent;
			point2 anchor_offset;
			point2 size_absolute;
			vector2 size_relative;
		};
	}
}