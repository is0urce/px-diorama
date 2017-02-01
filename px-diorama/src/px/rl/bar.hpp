// name: bar.hpp
// type: c++ header
// desc: template struct
// auth: is0urce

// resource bar
// value that have maximal cap
// starting value is maximal

#ifndef pragma once

namespace px
{
	namespace rl
	{
		template <typename T>
		class bar
		{
		public:
			typedef T value_type;

		private:
			T m_current;
			T m_max;

			// ctor & dtor
		public:
			bar() noexcept 
				: m_current{}, m_max{}
			{
			}
			bar(T max) noexcept
				: m_current(max), m_max(max)
			{
			}
			bar(T current, T max) noexcept
			{
				init(current, max);
			}

		private:
			void init(T current, T max) noexcept
			{
				m_max = max;
				m_current = cap(current);
			}
			T cap(T val) noexcept
			{
				return std::min<T>(val, m_max);
			}


		public:
			// querry

			T current() const noexcept
			{
				return m_current;
			}
			T maximum() const noexcept
			{
				return m_max;
			}
			bool empty() const noexcept
			{
				return m_current <= T{};
			}
			bool full() const noexcept
			{
				return m_current == m_max;
			}

			void current(T current) noexcept
			{
				m_current = cap(current);
			}
			void maximum(T max) noexcept
			{
				m_max = max;
				m_current = cap(m_current);
			}
			void set(T current_and_max) noexcept
			{
				init(current_and_max, current_and_max);
			}
			void set(T current, T max) noexcept
			{
				init(current, max);
			}
			// restore value to maximum
			void restore() noexcept
			{
				m_current = m_max;
			}
			void restore(T magnitude) noexcept
			{
				m_current = cap(m_current + magnitude);
			}
			void modify(T magnitude) noexcept
			{
				m_current = cap(m_current + magnitude);
			}
			void damage(T magnitude) noexcept
			{
				m_current -= magnitude;
			}

			// mutation operators

			bar& operator=(bar&) noexcept = default;
			bar& operator=(T c) noexcept
			{
				set(c);
				return *this;
			}
			bar& operator+=(T c) noexcept
			{
				modify(c);
				return *this;
			}
			bar& operator-=(T c) noexcept
			{
				modify(-c);
				return *this;
			}

			// comparison operators

			bool operator<(T c) const noexcept
			{
				return m_current < c;
			}
			bool operator==(T c) const noexcept
			{
				return m_current == c;
			}
			bool operator<=(T c) const noexcept
			{
				return operator<(*this, c) || operator==(this*, c);
			}
			bool operator>=(T c) const noexcept
			{
				return !operator<(*this, c);
			}
			bool operator!=(T c) const noexcept
			{
				return !operator==(*this, c);
			}
			bool operator>(T c) const noexcept
			{
				return !operator<=(*this, c);
			}

			// value cast operators

			operator value_type() const noexcept
			{
				return m_current;
			}
			operator bool() const noexcept
			{
				return m_current > T{};
			}
		};
	}
}

#endif