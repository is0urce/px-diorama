// name: inventory_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "../es/body_component.hpp"

#include "item_functional.hpp"

#include <px/rl/loot/inventory.hpp>

#include <px/ui/board.hpp>
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
				make<board>("background", fill, color{ 1, 1, 1, 0.5 });
				auto label = make<text>("label", { { 0.0, 0.0 },{ 0, 0 },{ 0, 1 },{ 1.0, 0.0 } }, "Inventory");
				auto inventory = make<list<rl::inventory>>({ { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 1.0, 1.0 } });
				inventory->set_format<item_name>();

				m_inventory = inventory.get();
			}
		private:
			list<rl::inventory> * m_inventory;
			body_component * m_equipment;
			container_component * m_container;
		};
	}
}