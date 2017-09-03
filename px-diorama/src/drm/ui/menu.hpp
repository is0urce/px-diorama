// name: menu.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include <px/common/assert.hpp>
#include <px/ui/panel.hpp>

#include <memory>

namespace px {
	
	class environment;

	class transform_component;
	class body_component;
	class container_component;
	class character_component;

	namespace ui {

		class target_panel;
		class inventory_panel;
		class storage_panel;
		class workshop_panel;
		class editor_panel;

		class menu final
		{
		public:
			panel * main() px_noexcept;
			panel const* main() const px_noexcept;

			void assign_environment(environment * env) noexcept;
			void assign_incarnation(transform_component * pawn) noexcept;
			void assign_target(transform_component * pawn, point2 absolute) noexcept;

			void open_storage(container_component * storage);
			void open_storage(container_component * storage, container_component * inspector);
			void open_workshop(container_component * user);

			void close_sheets();
			bool close_transactions();

			void break_links() noexcept;

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
			container_component *	m_container;	// incarnation container (inventory)
			character_component *	m_character;	// incarnation character (skills)

			target_panel *			m_status;		// incartation status
			target_panel *			m_target;		// target status
			inventory_panel *		m_inventory;	// inventory panel
			storage_panel *			m_storage;		// storage panel for inspecting containers
			workshop_panel *		m_workshop;		// workshop crafting
			editor_panel *			m_editor;		// ingame editor
		};
	}
}