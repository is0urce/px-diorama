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
	class body_component;
	class container_component;
	class character_component;

	namespace ui {

		class target_panel;
		class inventory_panel;

		class menu final
		{
		public:
			typedef list<rl::inventory> inventory_list;

		public:
			panel * main() noexcept;
			panel const* main() const noexcept;

			void assign_incarnation(transform_component * pawn);
			void assign_target(transform_component * pawn, point2 absolute);

			void open_storage(container_component * storage);
			void open_storage(container_component * storage, container_component * inspector);
			void open_workshop(container_component * user);

			void close_sheets();
			void break_links();
			void close_transactions();

		public:
			~menu();
			menu();
			menu(menu const&) = delete;
			menu & operator=(menu const&) = delete;

		private:
			void toggle_inventory();
			void initialize();

		private:
			std::unique_ptr<panel>	m_main;			// hierarchy root

			transform_component *	m_transform;	// incarnation transform
			body_component *		m_body;			// incarnation body (equipment)
			container_component *	m_storage;		// incarnation container (inventory)
			character_component *	m_character;	// incarnation character (skills)

			inventory_list *		m_container;
			inventory_list *		m_inspector;

			target_panel *			m_status;		// incartation status
			target_panel *			m_target;		// target status
			inventory_panel *		m_inventory;	// inventory panel
		};
	}
}