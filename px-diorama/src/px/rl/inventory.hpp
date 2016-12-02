// name: inventory.hpp
// type: c++ header
// desc: template class definition
// auth: is0urce

#pragma once

#include <px/rl/item.hpp>

#include <list>
#include <memory>

namespace px
{
	namespace rl
	{
		template <typename Effect>
		class inventory
		{
		public:
			typedef Effect effect_type;
			typedef item<effect_type> item_type;
			typedef std::shared_ptr<item_type> item_ptr;

		public:
			void add(item_ptr i)
			{
				for (item_ptr stack : m_items)
				{
					if (stack->can_stack(*i))
					{
						stack->stack(*i);

						if (i->stack_size() == 0) return;
					}
				}
				m_items.push_back(i);
			}
			void take_all(inventory &container)
			{
				for (item_ptr loot : container.m_items)
				{
					add(loot);
				}
				container.clear();
			}
			void remove(const item_ptr &i)
			{
				m_items.remove(i);
			}
			item_ptr remove(item_ptr i, unsigned int n)
			{
				auto part = std::make_shared<item_type>();

				if (i->split(n, *part) == 0) remove(i);

				return part;
			}
			void clear()
			{
				m_items.clear();
			}
			bool empty() const
			{
				return m_items.empty();
			}
			template <typename UnaryOperation>
			void enumerate(UnaryOperation&& enum_fn) const
			{
				for (const auto &item : m_items)
				{
					std::forward<UnaryOperation>(enum_fn)(item);
				}
			}

		private:
			std::list<item_ptr> m_items;
		};
	}
}