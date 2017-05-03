// name: menu.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

// game logics

#pragma once

#include <px/common/point.hpp>
#include <px/common/rectangle.hpp>

#include <memory>

namespace px {

	class transform_component;
	class container_component;

	namespace ui {

		class panel;

		class inventory_list;
		class recipe_list;
		class target_panel;

		class menu
		{
		public:
			panel * main() noexcept;
			panel const* main() const noexcept;

			void lock_target(point2 absolute, transform_component const* pawn);

			void open_inventory(container_component * user);
			void open_storage(container_component * storage, container_component * user);
			void open_workshop(container_component * user);
			void close_storage();
			void close_workshop();

			void close_sheets();

		public:
			~menu();
			menu();
			menu(menu const&) = delete;
			menu& operator=(menu const&) = delete;

		private:
			void initialize();

		private:
			std::unique_ptr<panel> m_main;
			inventory_list * m_container;
			inventory_list * m_inventory;
			target_panel * m_target;
		};
	}
}