// name: terrain_generator.hpp
// type: c++
// auth: is0urce
// desc: class template

#pragma once

#include <px/common/matrix.hpp>
#include <px/fn/cellular_automata.hpp>

#include <cstdint>
#include <random>

namespace px {

	template <size_t Width, size_t Height>
	class terrain_generator
	{
	public:
		typedef matrix2<uint32_t, Width, Height> matrix_type;

	public:
		static void generate_cavern(matrix_type & map)
		{
			std::mt19937 rng;
			fn::cellular_automata<unsigned char, Width, Height> automata([&](size_t /*x*/, size_t /*y*/) -> unsigned char { return rng() % 2; });
			automata.mutate<unsigned char>(4, 0, [](auto acc, auto cell) -> unsigned char { return acc + cell; }, [](auto x) -> unsigned char { return (x >= 5) ? 1 : 0; });

			map.enumerate([&](auto const& relative, auto & tile) {
				switch (automata->at(relative)) {
				case 1: {
					tile = (std::rand() % 3 == 0) ? 2 : 3;
					break;
				}
				default:
					tile = 1;
					break;
				}
			});
		}
		static matrix_type generate_cavern()
		{
			matrix_type result;
			generate_cavern(result);
			return result;
		}
	};
}
