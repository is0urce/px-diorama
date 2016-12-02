// name: mass.hpp
// type: c++ header
// desc: tile class
// auth: is0urce

// base of shared tile and unit proprieties
// transparensy and multiple layers of traversability

#pragma once

#include <bitset>

namespace px
{
	namespace rl
	{
		template <size_t Layers = 1>
		class mass
		{
		public:
			typedef std::bitset<Layers> bitset_type;

		public:
			// transparency accessors

			bool transparent() const noexcept { return m_transparent; }
			void make_transparent(bool transparency) noexcept { m_transparent = transparency; }
			void make_transparent() noexcept { m_transparent = true; }
			void make_opaque() noexcept { m_transparent = false; }

			// traverable getters

			bool traversable() const { return m_traversable.test(0); }
			bool traversable(size_t layer) const { return m_traversable.test(layer); }
			bool traversable(bitset_type l) const { return (m_traversable & l).any(); }
			template <typename E>
			bool traversable(E layer) const { return traversable(static_cast<size_t>(layer)); }

			// traversable setters

			void make_traversable(size_t layer, bool val)	{ m_traversable.set(layer, val); }
			void make_traversable(size_t layer) { m_traversable.set(layer); }
			void make_traversable(bool val)	{ m_traversable.set(0, val); }
			void make_traversable() { m_traversable.set(0); }
			void make_traversable(bitset_type layers) { m_traversable |= layers; }
			void make_blocking() { m_traversable.reset(0); }
			void make_blocking(size_t layer) { m_traversable.reset(layer); }
			void make_blocking(bitset_type layers) { m_traversable &= layers; }
			void make_traversable(bitset_type layers, bool val)
			{
				if (val)
				{
					m_traversable |= layers;
				}
				else
				{
					m_traversable &= ~layers;
				}
			}
			template <typename E>
			void make_traversable(E layer) { m_traversable.set(static_cast<size_t>(layer)); }
			template <typename E>
			void make_traversable(E layer, bool val) { traversable(static_cast<size_t>(layer), val); }
			template <typename E>
			void make_blocking(E layer) { m_traversable.reset(static_cast<size_t>(layer)); }

			// utility
			void make_wall() { make_blocking(); make_opaque(); }
			void make_ground() { make_traversable(); make_transparent(); }

		public:
			mass() {}
			mass(bool is_transparent) : m_transparent(is_transparent) {}
			mass(bool is_transparent, bool is_traversable) : m_transparent(is_transparent) { make_traversable(is_traversable); }
			mass(bool is_transparent, bitset_type is_traversable) : m_transparent(is_transparent), m_traversable(is_traversable) {}

		private:
			bool m_transparent;
			std::bitset<Layers> m_traversable;
		};
	}
}