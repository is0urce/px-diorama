// name: mass.hpp
// type: c++ header
// desc: tile class
// auth: is0urce

// base of shared tile and unit proprieties
// transparensy and multiple layers of traversability

#pragma once

#include <px/rl/traverse_options.hpp>

namespace px
{
	namespace rl
	{
		template <typename E, size_t Size = static_cast<size_t>(E::max_value)>
		class mass
			: public traverse_options<E, Size>
		{
		public:
			bool transparent() const noexcept { return m_transparent; }
			void make_transparent(bool transparency) noexcept { m_transparent = transparency; }
			void make_transparent() noexcept { m_transparent = true; }
			void make_opaque() noexcept { m_transparent = false; }

			// utility
			void make_wall() { make_blocking(); make_opaque(); }
			void make_ground() { make_traversable(); make_transparent(); }

		public:
			mass()
				: m_transparent(true)
			{}
			mass(bool is_transparent)
				: m_transparent(is_transparent)
			{}
			mass(bool is_transparent, bool is_traversable)
				: m_transparent(is_transparent)
			{
				make_traversable(is_traversable);
			}
			mass(bool is_transparent, traverse_options const& opts)
				: traverse_options(opts), m_transparent(is_transparent)
			{}

		private:
			bool m_transparent;
		};
	}
}