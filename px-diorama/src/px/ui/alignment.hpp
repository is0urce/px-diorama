// name: alignment.hpp
// type: c++
// desc: struct declaration
// auth: is0urce

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

			alignment() noexcept
			{
			}
			alignment(vector2 a_percent, point2 a_offset, point2 s_absolute, vector2 s_relative) noexcept
				: anchor_percent(a_percent)
				, anchor_offset(a_offset)
				, size_absolute(s_absolute)
				, size_relative(s_relative)
			{
			}
			alignment(vector2 a_percent, point2 a_offset, point2 s_absolute) noexcept
				: anchor_percent(a_percent)
				, anchor_offset(a_offset)
				, size_absolute(s_absolute)
				, size_relative(0, 0)
			{
			}
			alignment(
				vector2::component a_percent_x, vector2::component a_percent_y,
				point2::component a_offset_x, point2::component a_offset_y,
				point2::component s_absolute_x, point2::component s_absolute_y,
				vector2::component s_relative_x, vector2::component s_relative_y) noexcept
				: anchor_percent(a_percent_x, a_percent_y)
				, anchor_offset(a_offset_x, a_offset_y)
				, size_absolute(s_absolute_x, s_absolute_y)
				, size_relative(s_relative_x, s_relative_y)
			{
			}
			static alignment fill() noexcept
			{
				return{ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } };
			}
			static alignment empty() noexcept
			{
				return{ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.0, 0.0 } };
			}

			rectangle calculate(const rectangle &parent) const noexcept
			{
				point2 start = anchor_offset + parent.start() + (anchor_percent * parent.range()).ceil();
				point2 range = size_absolute + (size_relative * parent.range()).ceil();
				return{ start, range };
			}
			rectangle calculate() const noexcept
			{
				return{ anchor_offset, size_absolute };
			}
		};
	}
}