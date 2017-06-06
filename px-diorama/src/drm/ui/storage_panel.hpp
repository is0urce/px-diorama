// name: storage_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "item_functional.hpp"

#include "../es/container_component.hpp"

#include <px/common/assert.hpp>
#include <px/rl/loot/inventory.hpp>

#include <px/ui/board.hpp>
#include <px/ui/list.hpp>
#include <px/ui/panel.hpp>

namespace px {
	namespace ui {

		class storage_panel
			: public panel
		{
		public:
			void assign_containers(container_component * storage, container_component * inspector)
			{
				px_assert(m_storage);
				px_assert(m_inspector);

				m_storage->assign_container(storage);
				m_inspector->assign_container(inspector);
			}
			void break_links() noexcept
			{
				m_storage->detach_container();
				m_inspector->detach_container();
			}

		public:
			virtual ~storage_panel()
			{
			}
			storage_panel()
				: m_storage(nullptr)
				, m_inspector(nullptr)
			{
				// inspect container panel block
				make<board>(fill, color{ 1, 1, 1, 0.75 });
				m_storage = make<list<rl::inventory>>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } }).get();
				m_inspector = make<list<rl::inventory>>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } }).get();

				m_storage->on_click(item_transfer<list<rl::inventory>*>(m_storage, m_inspector));
				m_storage->set_format<item_name>();
				m_inspector->on_click(item_transfer<list<rl::inventory>*>(m_inspector, m_storage));
				m_inspector->set_format<item_name>();
			}

		private:
			list<rl::inventory> * m_storage;
			list<rl::inventory> * m_inspector;
		};
	}
}