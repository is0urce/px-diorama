// name: cellular_automata.hpp
// type: c++ header
// desc: template class for cellular automation
// auth: is0urce

#pragma once

#include <px/common/matrix.hpp>

#include <memory>

namespace px
{
	namespace fn
	{
		// A acc type
		// F fold operator
		// S select operator

		template <typename Cell, size_t W, size_t H>
		class cellular_automata final
		{
		public:
			typedef Cell cell_type;
			typedef matrix2<Cell, W, H> map_type;

		public:
			// run generator for several generations
			template <typename A, typename F, typename S>
			void mutate(unsigned int generations, A accumulator, F && fold, S && select)
			{
				for (unsigned int i = 0; i != generations; ++i) {
					next_generation(accumulator, fold, select);
				}
			}

			// run generator once
			template <typename A, typename F, typename S>
			void next_generation(A accumulator_start, F && fold, S && select)
			{
				// generate and swap
				m_map = std::make_unique<map_type>([&](size_t i, size_t j) {

					A accumulator = accumulator_start;
					point2 pos(static_cast<int>(i), static_cast<int>(j));

					accumulator = fold(accumulator, sample(pos));
					accumulator = fold(accumulator, sample(pos.moved({ 0, 1 })));
					accumulator = fold(accumulator, sample(pos.moved({ 0, -1 })));
					accumulator = fold(accumulator, sample(pos.moved({ -1, 0 })));
					accumulator = fold(accumulator, sample(pos.moved({ 1, 0 })));
					accumulator = fold(accumulator, sample(pos.moved({ 1, 1 })));
					accumulator = fold(accumulator, sample(pos.moved({ -1, -1 })));
					accumulator = fold(accumulator, sample(pos.moved({ 1, -1 })));
					accumulator = fold(accumulator, sample(pos.moved({ -1, 1 })));

					return select(accumulator);
				});
			}

			// array access
			map_type const* operator->() const noexcept
			{
				return m_map.get();
			}
			map_type const* map() const noexcept
			{
				return m_map.get();
			}

		public:
			cellular_automata()
				: m_map(std::make_unique<map_type>())
			{
			}
			cellular_automata(cell_type const& initial)
				: m_map(std::make_unique<map_type>(initial))
			{
			}
			template <typename Generator>
			cellular_automata(Generator fn)
				: m_map(std::make_unique<map_type>(fn))
			{
			}


		private:
			// clip out-of-range access to bounds
			cell_type const& sample(point2 const& position) const
			{
				return (*m_map)[position.clamped({ 0, 0 }, { W - 1, H - 1 })];
			}

		private:
			std::unique_ptr<map_type> m_map;
		};
	}
}