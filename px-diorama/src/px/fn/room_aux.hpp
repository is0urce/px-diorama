// name: room_aux.hpp
// type: c++
// desc: auxilary template functions
// auth: is0urce

#pragma once

#include "random_aux.hpp"

namespace px
{
	namespace fn
	{
		namespace
		{
			// get random point in a rectangle
			template <typename Random>
			point2 random_border_point(Random & rng, rectangle const& rect)
			{
				int w = rect.width();
				int h = rect.height();

				int index = std::uniform_int_distribution<int>{ 0, (w + h - 4) * 2 - 1};
				bool odd = index % 2 == 0;
				index = index / 2 + 1;

				if (odd)
				{
					return index < w - 2 ? rect.start().moved_axis<0>(index) : rect.start().moved_axis<1>(index);
				}
				else
				{
					return index < w - 2 ? rect.corner().moved_axis<0>(-index) : rect.start().moved_axis<1>(-index);
				}
			}
			// get random range in bigger range (inclusive, both 0 and max)
			template <typename Random>
			point2 random_range(Random & rng, point2 const& range)
			{
				return point2(std::uniform_int_distribution<point2::component>{ 0, range.x() }(rng), std::uniform_int_distribution<point2::component>{ 0, range.y() }(rng));
			}
			template <typename Random>
			point2 random_range(Random & rng, point2 const& min, point2 const& max)
			{
				return point2(std::uniform_int_distribution<point2::component>{ min.x(), max.x() }(rng), std::uniform_int_distribution<point2::component>{ min.y(), max.y() }(rng));
			}
			// get random point in a rectangle
			template <typename Random>
			point2 random_point(Random & rng, rectangle const& rect)
			{
				return rect.start() + point2(std::uniform_int_distribution<point2::component>(0, rect.range().x() - 1)(rng), std::uniform_int_distribution<point2::component>(0, rect.range().y() - 1)(rng));
			}
		}
	}
}