// name: item.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/enhancement_collection.hpp>
#include <px/rl/entity.hpp>

#include <string>

namespace px
{
	namespace rl
	{
		template <typename Effect>
		class item
			: public entity
			, public enhancement_collection<Effect>
		{
		public:
			typedef Effect effect_type;
			typedef enhancement_collection<Effect> collection_type;

		private:
			unsigned int m_stack;
			unsigned int m_max_stack; // default is 1, if max_stack is 0, allow unlimited stacking

		public:
			item() : m_stack(1), m_max_stack(1) {}

		public:
			unsigned int stack_size() const
			{
				return m_stack;
			}
			bool stack(item& i)
			{
				if (&i == this) throw std::runtime_error("px::rl::item::stack(item&) - stacking into same item");

				bool full = false;
				auto total = m_stack + i.m_stack;

				if (m_max_stack == 0 || total <= m_max_stack)
				{
					m_stack = total;
					i.m_stack = 0;
					full = true;
				}
				else
				{
					m_stack = m_max_stack;
					i.m_stack -= total - m_stack;
				}
				return full;
			}
			unsigned int split(unsigned int n, item& into)
			{
				into = *this;

				if (n == 0) return;
				if (n > m_stack) n = m_stack;

				m_stack -= n;
				into.m_stack = n;

				return m_stack;
			}
			bool can_stack(item& i) const
			{
				if (&i == this) return false; // stacking into same item
				if (m_stack == m_max_stack) return false;
				if (name() != i.name() || tag() != i.tag()) return false;

				return static_cast<collection_type>(*this) == static_cast<collection_type>(i);
			}

			// unlimited max size
			void make_stacking()
			{
				m_max_stack = 0;
			}

			// size - maximum size of a stack, use 0 for unlimited
			void make_stacking(unsigned int size)
			{
				m_max_stack = size;
			}
		};
	}
}