// name: tile_chunk.hpp
// type: c++
// auth: is0ruce
// desc: class template

#pragma once

#include <px/common/matrix.hpp>



namespace px {

	template <typename Tile, size_t W, size_t H>
	class tile_chunk
	{
	public:
		typedef Tile tile_type;

	public:
		bool traversable(point2 const& position) const noexcept
		{
			return m_matrix.contains(position) && m_matrix[position].mass.traversable();
		}
		tile_type const& operator[](point2 const& relative) const
		{
			return m_matrix[relative];
		}
		void generate()
		{
			point2 range = m_matrix.range();

			//fn::dig_generator dig(range.x(), range.y());
			//dig.generate(std::mt19937{}, 4, 7, 1, 15);

			std::mt19937 rng;
			fn::cellular_automata<unsigned char, 100, 100> automata([&](size_t /*x*/, size_t /*y*/) -> unsigned char { return rng() % 2; });
			automata.mutate<unsigned char>(4, 0
				, [](auto acc, auto cell) -> unsigned char { return acc + cell; }
				, [](auto x) -> unsigned char { return (x >= 5) ? 1 : 0; });

			//matrix2<unsigned char> digged_map(range);
			//dig.rasterize(digged_map);

			m_matrix.enumerate([&](auto const& point, auto & tile) {
				switch (automata->at(point)) {
				case 1: {
					tile.id = (std::rand() % 3 == 0) ? 2 : 3;
					//setup(tile, 1);
					break;
				}
				default:
					tile.id = 1;
					//setup(tile, std::rand() % 3 == 0 ? 2 : 3);
					break;
				}
			});
		}
		template <typename Operator>
		void enumerate(Operator && fn)
		{
			m_matrix.enumerate(std::forward<Operator>(fn));
		}

	public:
		tile_chunk()
		{
		}

	private:
		matrix2<tile_type, W, H> m_matrix;
	};
}
