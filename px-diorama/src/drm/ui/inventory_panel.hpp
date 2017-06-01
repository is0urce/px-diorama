// name: inventory_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "../es/body_component.hpp"

#include "item_functional.hpp"

#include <px/rl/loot/inventory.hpp>

#include <px/ui/board.hpp>
#include <px/ui/button.hpp>
#include <px/ui/list.hpp>
#include <px/ui/panel.hpp>
#include <px/ui/text.hpp>

namespace px {
	namespace ui {

		class inventory_panel
			: public panel
		{
		public:
			void assign_container(body_component * equipment)
			{
				assign_container(equipment, equipment ? equipment->linked<container_component>() : nullptr);
			}
			void assign_container(body_component * equipment, container_component * container)
			{
				m_equipment = equipment;
				m_container = container;
				m_inventory->assign_container(container);
			}
			void break_links()
			{
				m_equipment = nullptr;
				m_container = nullptr;
			}

		public:
			virtual ~inventory_panel()
			{
			}
			inventory_panel()
				: m_equipment(nullptr)
				, m_container(nullptr)
			{
				// inventory panel block
				auto inventory_block = make<panel>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } });
				inventory_block->make<board>(fill, color{ 1.0, 1.0, 1.0, 0.5 });
				inventory_block->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 1 },{ 1.0, 0.0 } }, "Inventory");
				m_inventory = inventory_block->make<list<rl::inventory>>({ { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 1.0, 1.0 } }).get();
				m_inventory->set_format<item_name>();
				m_inventory->on_click([this](auto & item) {
					if (!m_equipment) return;
					bool equipment;
					rl::item::enhancement_type enhancement;
					std::tie(equipment, enhancement) = item->find<rl::effect::equipment>();
					if (equipment) {
						m_equipment->equip(static_cast<rl::equipment_slot>(enhancement.subtype), *item);
						m_container->remove(item);
					}
				});

				make_slot({ { 0.0, 0.0 },{ 0, 0 },{ -1, 1 },{ 0.5, 0.0 } }, rl::equipment_slot::weapon_main, "!weapon");
				make_slot({ { 0.0, 0.0 },{ 0, 2 },{ -1, 1 },{ 0.5, 0.0 } }, rl::equipment_slot::armor_helmet, "!helmet");
				make_slot({ { 0.0, 0.0 },{ 0, 4 },{ -1, 1 },{ 0.5, 0.0 } }, rl::equipment_slot::armor_chest, "!armor");
			}

		private:
			void make_slot(alignment subpanel_alignment, rl::equipment_slot equipment_slot, std::string unequipped_text)
			{
				auto slot = make<panel>(subpanel_alignment);
				slot->make<board>("bg", fill, color{ 1, 1, 1, 0.5 });
				slot->make<text>(fill, [this, equipment_slot, unequipped_text]() -> std::string {
					if (m_equipment && m_equipment->equipped(equipment_slot)) {
						return m_equipment->at(equipment_slot).name();
					}
					return unequipped_text;
				});
				auto unequip_button = slot->make<button>(fill);
				unequip_button->on_click([this, equipment_slot](int /* mouse_button */) {
					if (m_equipment && m_equipment->equipped(equipment_slot)) {
						m_container->add(std::make_shared<rl::item>(m_equipment->at(equipment_slot))); // copy constructor emplacement
						m_equipment->unequip(rl::equipment_slot::weapon_main);
					}
				});
			}
		private:
			list<rl::inventory> * m_inventory;
			body_component * m_equipment;
			container_component * m_container;
		};
	}
}