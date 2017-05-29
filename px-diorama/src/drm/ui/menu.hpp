// name: menu.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include <px/common/point.hpp>
#include <px/common/rectangle.hpp>
#include <px/rl/loot/inventory.hpp>
#include <px/ui/list.hpp>

#include <memory>

namespace px {

	class transform_component;
	class container_component;

	namespace ui {

		class panel;
		class recipe_list;
		class target_panel;

		class menu
		{
		public:
			typedef list<rl::inventory> inventory_list;

		public:
			panel * main() noexcept;
			panel const* main() const noexcept;

			void lock_target(point2 absolute, transform_component const* pawn);

			void assign_inventory(container_component * user);
			void open_storage(container_component * storage, container_component * inspector);
			void open_workshop(container_component * user);

			void close_sheets();
			void break_links();

		public:
			~menu();
			menu();
			menu(menu const&) = delete;
			menu& operator=(menu const&) = delete;

		private:
			void toggle_inventory();
			void initialize();

		private:
			std::unique_ptr<panel>	m_main;
			container_component *	m_player;

			inventory_list *		m_inventory;
			inventory_list *		m_container;
			inventory_list *		m_inspector;

			target_panel *			m_target;
		};
	}
}