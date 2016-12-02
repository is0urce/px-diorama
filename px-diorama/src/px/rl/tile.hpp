// name: tile.hpp
// type: c++ header
// desc: tile class
// auth: is0urce

#pragma once

#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>

namespace px
{
	namespace rl
	{
		template <typename Image>
		class tile : public mass<static_cast<size_t>(traverse::max_value)>
		{
		public:
			typedef Image appearance_type;
			typedef mass<static_cast<size_t>(traverse::max_value)> mass_type;

		public:
			const appearance_type& appearance() const noexcept
			{
				return m_image;
			}
			appearance_type& appearance() noexcept
			{
				return m_image;
			}

		public:
			tile() noexcept {};
			tile(mass_type m, appearance_type img) noexcept : mass(m), m_image(img) {}

		private:
			Image m_image;
		};
	}
}