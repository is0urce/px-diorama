// name: workshop_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "item_functional.hpp"

#include <px/rl/loot/inventory.hpp>

#include <px/ui/board.hpp>
#include <px/ui/button.hpp>
#include <px/ui/list.hpp>
#include <px/ui/panel.hpp>
#include <px/ui/text.hpp>

#include <px/ui/panel.hpp>

#include <memory>
#include <vector>

namespace px {
	namespace ui {

		class workshop_panel
			: public panel
		{
		public:
			void assign_container(container_component * user_container) noexcept
			{
				m_container = user_container;
				m_inventory->assign_container(user_container);
			}
			void break_links() noexcept
			{
				m_container = nullptr;
				m_inventory->detach_container();
			}

		public:
			virtual ~workshop_panel()
			{
			}
			workshop_panel()
				: m_container(nullptr)
				, m_inventory(nullptr)
				, m_slots(nullptr)
			{
				// inventory panel block
				auto inventory_block = make<panel>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } });
				inventory_block->make<board>(fill, color{ 1.0, 1.0, 1.0, 0.5 });
				inventory_block->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 1 },{ 1.0, 0.0 } }, "Inventory");
				m_inventory = inventory_block->make<list<rl::inventory>>({ { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 1.0, 1.0 } }).get();
				m_inventory->set_format<item_name>();
				m_inventory->on_click([this](auto & item) {
					add_ingredient(item);
				});

				// slots kraft machine

				m_slots = make<panel>(fill).get();

				make_slots(10);
			}

		private:
			void make_slots(int total)
			{
				px_assert(m_slots);
				px_assert(total >= 0);

				m_slots->clear_anonimous(); // clear previous

				for (int i = 0; i != total; ++i) {

					// data
					std::shared_ptr<std::shared_ptr<rl::item>> slot_data = std::make_shared<std::shared_ptr<rl::item>>();
					m_slots_data.push_back(slot_data);

					// ui
					auto k_block = m_slots->make<board>({ { 0.0, 0.0 },{ 2,1 + i * 2 },{ 15, 1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
					auto slot_press = k_block->make<button>(fill);

					k_block->make<text>(fill, [slot_data]()->std::string {
						return (*slot_data) ? item_name::format(*slot_data) : "-- empty --";
					});
					slot_press->on_click([this, i](int /* mouse_button */) {
						remove_ingredient(i);
					});
				}
			}
			void add_ingredient(std::shared_ptr<rl::item> item)
			{
				// find first empty slot
				size_t size = m_slots_data.size();
				size_t slot_index = size;
				for (size_t i = 0; i != size; ++i) {
					if (!(*m_slots_data[i])) {
						slot_index = i;
						break;
					}
				}

				px_assert(m_container);

				if (m_container && slot_index < size) {

					*m_slots_data[slot_index] = m_container->take(item, 1);
				}
			}
			void remove_ingredient(size_t slot_index)
			{
				px_assert(m_container);

				if (m_container) {

					std::shared_ptr<rl::item> item = *m_slots_data[slot_index];
					if (item) {
						m_container->add(item);
						m_slots_data[slot_index]->reset();
					}
				}
			}

		private:
			container_component *	m_container;
			list<rl::inventory> *	m_inventory;
			panel *					m_slots;
			std::vector<std::shared_ptr<std::shared_ptr<rl::item>>> m_slots_data;
		};
	}
}