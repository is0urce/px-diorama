// name: doll.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

#include <map>

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
			void strip()
			{
				m_equipment.clear();
			}
			item_type & operator[](slot_type slot)
			{
				return m_equipment[slot];
			}
			item_type const& operator[](slot_type slot) const
			{
				return m_equipment[slot];
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