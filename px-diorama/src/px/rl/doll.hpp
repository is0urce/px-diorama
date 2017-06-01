// name: doll.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

#include <map>
#include <memory>

namespace px {
	namespace rl {

		template <typename Slot, typename Item>
		class doll
		{
		public:
			typedef Slot slot_type;
			typedef Item item_type;

		public:
			void equip(slot_type slot, item_type item)
			{
				m_equipment[slot] = item;
			}
			void unequip(slot_type slot)
			{
				m_equipment.erase(slot);
			}
			bool equipped(slot_type slot) const
			{
				auto find = m_equipment.find(slot);
				return find != m_equipment.end();
			}
			void strip()
			{
				m_equipment.clear();
			}
			item_type & operator[](slot_type slot)
			{
				return m_equipment[slot];
			}
			item_type const& at(slot_type slot) const
			{
				return m_equipment.at(slot);
			}
			template <slot_type Slot>
			item_type const& at() const
			{
				return m_equipment.at(Slot);
			}

			template <typename Inventory>
			void unequip(Inventory & container, slot_type slot)
			{
				auto find = m_equipment.find(slot);
				if (find != m_equipment.end()) {
					container.add(std::make_shared<item_type>(find->second)); // copy constructor emplacement
					m_equipment.erase(find);
				}
			}
			template <typename Inventory>
			void equip(Inventory & container, std::shared_ptr<item_type> item, slot_type slot)
			{
				auto find = m_equipment.find(slot);
				if (find != m_equipment.end()) {
					container.add(std::make_shared<item_type>(find->second));
					find->second = *item; // replace
				}
				else {
					m_equipment.emplace(slot, *item);
				}

				container.remove(item);
			}


			template <typename Operator>
			void enumerate(Operator && fn)
			{
				for (auto & kv_pair : m_equipment) {
					fn(kv_pair.second);
				}
			}
			template <typename Operator>
			void enumerate(Operator && fn) const
			{
				for (auto const& kv_pair : m_equipment) {
					fn(kv_pair.second);
				}
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_equipment);
			}

		private:
			std::map<slot_type, item_type> m_equipment;
		};
	}
}