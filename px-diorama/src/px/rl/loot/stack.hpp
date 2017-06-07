// name: stack.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

// base item class with operations for staking

namespace px
{
	namespace rl
	{
		class stack
		{
		public:
			// returns number of items failed to stack (due restrictions)
			// zero if everything is stacked
			unsigned int stack_from(stack & with) noexcept
			{
				if (&with == this) return m_stack; // stacking onto same item has no point

				unsigned int total = m_stack + with.m_stack;

				if (m_max_stack == 0 || total <= m_max_stack) {
					m_stack = total;
					with.m_stack = 0;
				}
				else {
					m_stack = m_max_stack;
					with.m_stack -= total - m_stack;
				}
				return with.m_stack;
			}

			// returns items left
			unsigned int split_into(unsigned int n, stack & into) noexcept
			{
				if (n == 0) return m_stack;

				if (n > m_stack) n = m_stack; // can't move more items than stack has

				into.m_max_stack = m_max_stack;
				into.m_stack = n;
				m_stack -= n;

				return m_stack;
			}

			unsigned int increase(unsigned int n) noexcept
			{
				unsigned int total = m_stack + n;
				return m_stack = (total < m_max_stack || m_max_stack == 0) ? total : m_max_stack;
			}
			unsigned int decrease(unsigned int n) noexcept
			{
				return m_stack -= (n < m_stack) ? n : m_stack;
			}

			unsigned int count() const noexcept
			{
				return m_stack;
			}
			unsigned int maximum() const noexcept
			{
				return m_max_stack;
			}
			bool full() const noexcept
			{
				return m_stack == m_max_stack;
			}
			bool last() const noexcept
			{
				return m_stack == 1;
			}
			bool single() const noexcept
			{
				return m_max_stack == 1;
			}
			bool stacking() const noexcept
			{
				return m_max_stack != 1;
			}

			void set_current_stack(unsigned int n) noexcept
			{
				m_stack = n;
			}

			// unlimited max size
			void make_stacking() noexcept
			{
				m_max_stack = 0;
			}

			// size - maximum size of a stack, use 0 for unlimited
			void make_stacking(unsigned int size) noexcept
			{
				m_max_stack = size;
			}

			// disallow stacking
			void make_single() noexcept
			{
				m_max_stack = 1;
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_stack, m_max_stack);
			}

		public:
			stack() noexcept
				: m_stack(1)
				, m_max_stack(1)
			{
			}
			stack(unsigned int stack, unsigned int max) noexcept
				: m_stack(stack)
				, m_max_stack(max)
			{
			}

		private:
			unsigned int m_stack;
			unsigned int m_max_stack; // default is 1, if max_stack is 0, allow unlimited stacking
		};
	}
}