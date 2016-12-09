// name: random_aux.hpp
// type: c++
// desc: auxilary template functions
// auth: is0urce

#pragma once

#include <px/common/rectangle.hpp>

#include <random>
#include <type_traits>

namespace px
{
	namespace fn
	{
		namespace
		{
			template <typename Random>
			point2 random_block_direction(Random & rng)
			{
				switch (std::uniform_int_distribution<int>{0, 3}(rng))
				{
				case 0: return{ 0, 1 };
				case 1: return{ 1, 0 };
				case 2: return{ -1, 0 };
				case 3: return{ 0, -1 };
				}
				throw std::logic_error("px::fn::random_block_direction");
			}

			// get random item from vector
			template <typename Random, typename Item>
			Item const& random_item(Random & rng, std::vector<Item> const& vec)
			{
				if (vec.empty()) throw std::runtime_error("px::fn::random_item(...) - vector size = 0");
				return vec[std::uniform_int_distribution<std::vector<Item>::size_type>(0, vec.size() - 1)(rng)];
			}
		}
	}
}