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
			void expose_inventory(container_component * storage);

		public:
			~menu();
			menu();

		private:
			void initialize();

		private:
			std::unique_ptr<panel> m_main;
			inventory_list * m_inventory;
			target_panel * m_target;
		};
	}
}