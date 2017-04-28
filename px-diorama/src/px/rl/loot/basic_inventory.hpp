// name: basic_inventory.hpp
// type: c++ header
// desc: template class definition
// auth: is0urce

#pragma once

#include <vector>
#include <stdexcept>

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
			void add(item_ptr & item)
			{
				if (!item) throw std::invalid_argument("px::basic_inventory::add(item) - item is null");

				// try to stack onto existing items
				for (auto & stack : m_items) {
					if (stack->try_stack_from(*item) == 0) break; // stacked fully onto existing item
				}

				// add remaining quantity to end of the list
				if (item->count() > 0) {
					m_items.push_back(item);
				}
			}
			void remove(item_ptr & item, unsigned int n)
			{
				for (auto it = std::begin(m_items), last = std::end(m_items); it != last; ++it) {
					if (*it == item && item->decrease(n) == 0) {
						m_items.erase(it);
						break;
					}
				}
			}
			void remove(item_ptr & item)
			{
				remove(item, 1);
			}
			void transfer(item_ptr & item, basic_inventory & destination)
			{
				if (&destination == this) return;

				for (auto it = std::begin(m_items), last = std::end(m_items); it != last;++it) {
					if (*it == item) {
						destination.add(item);
						m_items.erase(it);
						break;
					}
				}
			}
			void transfer(basic_inventory & destination)
			{
				if (&destination == this) return;

				// move one by one, so items can stack
				for (auto & loot : m_items) {
					destination.add(loot);
				}
				clear();
			}

			// enumeration
			template <typename UnaryOperator>
			void enumerate(UnaryOperator && enum_fn) const
			{
				for (auto const& item : m_items) {
					std::forward<UnaryOperator>(enum_fn)(item);
				}
			}
			template <typename UnaryOperator>
			void enumerate(UnaryOperator && enum_fn)
			{
				for (auto & item : m_items)	{
					std::forward<UnaryOperator>(enum_fn)(item);
				}
			}

			template <typename UnaryOperator>
			bool enumerate_while(UnaryOperator && enum_fn) const
			{
				for (auto it = std::begin(m_items), last = std::end(m_items); it != last; ++it) {
					if (!std::forward<UnaryOperator>(enum_fn)(*it)) return false;
				}
				return true;
			}
			template <typename UnaryOperator>
			bool enumerate_while(UnaryOperator && enum_fn)
			{
				for (auto it = std::begin(m_items), last = std::end(m_items); it != last; ++it) {
					if (!std::forward<UnaryOperator>(enum_fn)(*it)) return false;
				}
				return true;
			}

		private:
			std::vector<item_ptr> m_items;
		};
	}
}