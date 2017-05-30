// name: enhancement.hpp
// type: c++ header
// desc: class
// auth: is0urce

#pragma once

#include <cstdint>

namespace px {
	namespace rl {

		template <typename Effect>
		class enhancement final
		{
		public:
			typedef Effect effect_type;
			typedef uint32_t int_t;
			typedef double float_t;

		public:
			static enhancement integer(effect_type general_type, int_t main_value)
			{
				return enhancement{ general_type, main_value };
			}
			static enhancement integer(effect_type general_type, int_t variation, int_t main_value, int_t complimentary_value)
			{
				return enhancement{ general_type, variation, main_value, complimentary_value };
			}

			enhancement & operator+=(enhancement const& rhs)
			{
				value0 += rhs.value0;
				value1 += rhs.value1;
				magnitude0 += rhs.magnitude0;
				magnitude1 += rhs.magnitude1;
				main_type = rhs.main_type;
				subtype = rhs.subtype;
				return *this;
			}
			enhancement & operator*=(enhancement const& rhs)
			{
				value0 *= rhs.value0;
				value1 *= rhs.value1;
				magnitude0 *= rhs.magnitude0;
				magnitude1 *= rhs.magnitude1;
				main_type = rhs.main_type;
				subtype = rhs.subtype;
				return *this;
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(main_type, subtype, value0, value1, magnitude0, magnitude1, disabled, hidden);
			}

			enhancement()
				: subtype(0)
				, value0(0)
				, value1(0)
				, magnitude0(1)
				, magnitude1(1)
				, disabled(false)
				, hidden(false)
			{
			}
			enhancement(effect_type general_type, int_t main_value)
				: main_type(general_type)
				, subtype(0)
				, value0(main_value)
				, value1(main_value)
				, magnitude0(1)
				, magnitude1(1)
				, disabled(false)
				, hidden(false)
			{
			}
			enhancement(effect_type general_type, int_t variation, int_t main_value, int_t complimentary_value)
				: main_type(general_type)
				, subtype(variation)
				, value0(main_value)
				, value1(complimentary_value)
				, magnitude0(1)
				, magnitude1(1)
				, disabled(false)
				, hidden(false)
			{
			}
			enhancement(effect_type general_type, float_t float_value)
				: main_type(general_type)
				, subtype(0)
				, value0(0)
				, value1(0)
				, magnitude0(float_value)
				, magnitude1(float_value)
				, disabled(false)
				, hidden(false)
			{
			}

		public:
			int_t			subtype;	// can vary depending on main type, so use cast to int type
			int_t			value0;
			int_t			value1;
			float_t			magnitude0;
			float_t			magnitude1;
			bool			disabled;	// should not provide bonuses if this flag is set
			bool			hidden;		// invisible to players
			effect_type		main_type;	// type and subtype of enhancement
		};

		namespace
		{
			template <typename E>
			inline bool operator==(enhancement<E> const& lh, enhancement<E> const& rh) noexcept
			{
				return std::memcmp(&lh, &rh, sizeof(enhancement<E>)) == 0;
			}
			template <typename E>
			inline bool operator!=(enhancement<E> const& lh, enhancement<E> const& rh) noexcept
			{
				return !operator=(lh, rh);
			}
		}
	}
}