// name: enhancement.hpp
// type: c++ header
// desc: struct
// auth: is0urce

#pragma once

namespace px
{
	namespace rl
	{
		template<typename Effect>
		struct enhancement final
		{
		public:
			typedef Effect effect_type;

		public:
			// type and subtype of enhancement

			effect_type main_type;
			unsigned int subtype; // can vary depending on main type, so use cast to int type

			// power of enhancement

			int value0;
			int value1;
			double magnitude0;
			double magnitude1;

			// flags

			bool disabled; // it should not provide bonuses if this flag is set
			bool hidden; // is it invisible to players

		public:
			enhancement& operator+=(enhancement const& rhs)
			{
				value0 += rhs.value0;
				value1 += rhs.value1;
				magnitude0 += rhs.magnitude0;
				magnitude1 += rhs.magnitude1;
				subtype = rhs.subtype;
				return *this;
			}
			enhancement& operator*=(enhancement const& rhs)
			{
				value0 *= rhs.value0;
				value1 *= rhs.value1;
				magnitude0 *= rhs.magnitude0;
				magnitude1 *= rhs.magnitude1;
				subtype = rhs.subtype;
				return *this;
			}
		};

		namespace
		{
			template <typename E>
			bool operator==(enhancement<E> const& lh, enhancement<E> const& rh) noexcept
			{
				return std::memcmp(&lh, &rh, sizeof(enhancement<E>)) == 0;
			}
			template <typename E>
			bool operator!=(enhancement<E> const& lh, enhancement<E> const& rh) noexcept
			{
				return !operator=(lh, rh);
			}
		}
	}
}