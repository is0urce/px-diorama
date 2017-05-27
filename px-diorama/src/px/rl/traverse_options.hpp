// name: traverse_options.hpp
// type: c++ header
// desc: template class
// auth: is0urce

// class for typed traverse options bitset

#pragma once

#include <bitset>

namespace px
{
	namespace rl
	{
		template <typename E, size_t Size = static_cast<size_t>(E::max_value)>
		class traverse_options
		{
		public:
			typedef E enumeration_type;
		public:
			static const size_t traverse_layers = Size;

		public:
			// traverable getters

			bool traversable() const { return m_traversable.test(0); }
			bool traversable(traverse_options opts) const { return (m_traversable & opts.m_traversable).any(); }
			bool traversable(E layer) const { return m_traversable.test(static_cast<size_t>(layer)); }

			// traversable setters

			void make_traversable() { m_traversable.set(0); }
			void make_traversable(bool val) { m_traversable.set(0, val); }
			void make_traversable(E layer) { m_traversable.set(static_cast<size_t>(layer)); }
			void make_traversable(E layer, bool val) { m_traversable.set(static_cast<size_t>(layer), val); }
			void make_traversable(traverse_options const& opts) { m_traversable |= opts.m_traversable; }
			void make_blocking() { m_traversable.reset(0); }
			void make_blocking(E layer) { m_traversable.reset(static_cast<size_t>(layer)); }
			void make_blocking(traverse_options const& opts) { m_traversable &= opts.m_traversable; }
			void make_traversable(traverse_options const& layer_options, bool val)
			{
				if (val)
				{
					m_traversable |= layer_options.m_traversable.layers;
				}
				else
				{
					m_traversable &= ~layer_options.m_traversable.layers;
				}
			}

			//static constexpr size_t traverse_layers() noexcept
			//{
			//	return Size;
			//}

		public:
			traverse_options()
			{
			}
			traverse_options(std::bitset<Size> bits)
				: m_traversable(bits)
			{
			}

		private:
			std::bitset<Size> m_traversable;
		};
	}
}