// name: item_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

// panel for inspecting item attributes

#include <px/ui/panel.hpp>

#include <px/rl/loot/item.hpp>
#include <px/rl/effect.hpp>

#include <px/rl/enhancement_inspector.hpp>

#include "item_functional.hpp"

#include <memory>

namespace px {
	namespace ui {

		class item_panel
			: public panel
		{
		public:
			typedef rl::item item_type;

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
				: m_item(nullptr)
			{
				m_inspector.add(rl::effect::weapon_damage, "Damage", [](int v0, int /* v1 */, double /* m0 */, double /* m1 */) { return std::to_string(v0); }, rl::enhancement_status::general);
			}

		private:
			item_type const* m_item;
			rl::enhancement_inspector<rl::effect> m_inspector;
		};
	}
}