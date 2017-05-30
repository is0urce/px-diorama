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

		class menu final
		{
		public:
			typedef list<rl::inventory> inventory_list;

		public:
			panel * main() noexcept;
			panel const* main() const noexcept;

			void assign_player(transform_component * pawn);
			void assign_target(transform_component * pawn, point2 absolute);

			void open_storage(container_component * storage);
			void open_storage(container_component * storage, container_component * inspector);
			void open_workshop(container_component * user);

			void close_sheets();
			void break_links();

		public:
			~menu();
			menu();
			menu(menu const&) = delete;
			menu & operator=(menu const&) = delete;

		private:
			void toggle_inventory();
			void initialize();

		private:
			std::unique_ptr<panel>	m_main;

			transform_component *	m_transform;
			body_component *		m_body;
			container_component *	m_storage;
			character_component *	m_character;

			inventory_list *		m_inventory;
			inventory_list *		m_container;
			inventory_list *		m_inspector;

			target_panel *			m_status;
			target_panel *			m_target;
		};
	}
}