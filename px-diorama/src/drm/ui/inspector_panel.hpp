// name: item_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include <px/rl/effect.hpp>
#include <px/rl/enhancement_inspector.hpp>

#include "item_functional.hpp"

namespace px {
	namespace ui {

		class item_panel
			: public panel
		{
		public:
			typedef void item_type;

		public:
			void assign_item(item_type const* item)
			{
				m_item = item;
			}
			void break_links() noexcept
			{
				m_item = nullptr;
			}

		public:
			virtual ~item_panel()
			{
			}
			item_panel()
			{
			}

		private:
			item_type const* m_item;
			rl::enhancement_inspector<rl::effect> m_inspector;
		};
	}
}