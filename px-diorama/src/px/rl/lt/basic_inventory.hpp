// name: basic_inventory.hpp
// type: c++ header
// desc: template class definition
// auth: is0urce

#pragma once

#include <vector>

namespace px
{
	namespace rl
	{
		template <typename Item>
		class basic_inventory
		{
		public:
			typedef Item item_type;
			typedef std::shared_ptr<item_type> item_ptr;

		public:
			bool empty() const noexcept
			{
				return m_items.empty();
			}
			void clear()
			{
				m_items.clear();
			}
			void add(item_ptr item)
			{
				for (auto & stack : m_items)
				{
					if (stack->try_stack(item) == 0) return; // stacked fully onto existing item
				}
				m_items.push_back(item);
			}
			void remove(item_ptr & item, unsigned int n)
			{
				for (auto it = std::begin(m_items), last = std::end(m_items); it != last; ++it)
				{
					if (*it == item)
					{
						if (item->desrease(n) == 0)
						{
							m_items.erase(it);
							return;
						}
					}
				}
			}
			void remove(item_ptr & item)
			{
				remove(item, 1);
			}
			void move_to(basic_inventory & to)
			{
				for (auto & loot : m_items)
				{
					to.add(loot);
				}
				clear();
			}

			template <typename UnaryOperator>
			void enumerate(UnaryOperator && enum_fn) const
			{
				for (const auto &item : m_items)
				{
					std::forward<UnaryOperator>(enum_fn)(item);
				}
			}

		private:
			std::vector<item_ptr> m_items;
		};
	}
}