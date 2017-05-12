// name: random_aux.hpp
// type: c++
// desc: auxilary template functions
// auth: is0urce

#pragma once

#include <px/common/rectangle.hpp>

#include <random>
#include <type_traits>

namespace px {
	namespace fn {

		namespace
		{
			// get random 4-way direction
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
				return vec[std::uniform_int_distribution<size_t>(0, vec.size() - 1)(rng)];
			}

			template <typename Random, typename Item, typename Predicate>
			Item const& random_item_filtered(Random & rng, std::vector<Item> const& vec, Predicate && predicate)
			{
				size_t size = std::accumulate(std::begin(vec), std::end(vec), size_t{}, [predicate = std::forward<Predicate>(predicate)](size_t n, Item const& element)->size_t {
					return n + (predicate(element) ? 1 : 0);
				});
				if (size == 0) throw std::runtime_error("px::fn::random_item_filtered(...) - not found any");
				size_t index = std::uniform_int_distribution<size_t>{ 0, size - 1 }(rng);

				for (size_t ui = 0, pi = 0, total = vec.size(); ui != total; ++ui)
				{
					if (!predicate(vec[ui])) continue;
					if (index == pi) return vec[ui];
					++pi;
				}

				throw std::logic_error("px::fn::random_item_filtered(...) - wrong indices");
			}
		}
	}
}