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

#include <px/ui/board.hpp>
#include <px/ui/text.hpp>

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
			void inspect(item_type const& item)
			{
				clear();

				m_name->set_text(item.name());
				item.enumerate([&](auto const& enhancement) {
					auto result = m_inspector.inspect(enhancement);
					if (result) {

						auto prop = m_properties->make<text>({ { 0.0, 0.0 },{ 0, 1 },{ 0, 0 },{ 1.0, 1.0 } }, "xxx");
						switch (result->status)
						{
						case rl::enhancement_status::positive:
							prop->set_color({ 0.0, 1.0, 0.0 });
							prop->set_text(result->value + " " + result->name);
							break;
						case rl::enhancement_status::negative:
							prop->set_color({ 0.0, 1.0, 0.0 });
							prop->set_text(result->value + " " + result->name);
						case rl::enhancement_status::neutral:
							prop->set_color({ 0.0, 0.0, 0.0 });
							prop->set_text(result->value + " " + result->name);
						case rl::enhancement_status::general:
							prop->set_color({ 0.0, 0.0, 0.0 });
							prop->set_text(result->name + " " + result->value);
						default:
							prop->set_color({ 0.0, 0.0, 0.0 });
							prop->set_text(result->name + ": " + result->value);
							break;
						}
					}
				});
			}

		public:
			virtual ~item_panel()
			{
			}
			item_panel()
				: m_name(nullptr)
				, m_properties(nullptr)
			{
				m_inspector.add(rl::effect::weapon_damage, "Damage", [](int v0, int /* v1 */, double /* m0 */, double /* m1 */) { return std::to_string(v0); }, rl::enhancement_status::general);

				// setup ui layout

				auto name_block = make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } }, "");
				auto properties_block = make<panel>({ { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 1.0, 1.0 } });

				m_properties = properties_block.get();
				m_name = name_block.get();

				clear();
			}

		private:
			void clear()
			{
				m_name->set_text("no item selected");
				m_properties->clear_anonimous();
			}

		private:
			text *				m_name;			// text subpanel cashed
			panel *				m_properties;	// porperties container cashed

			rl::enhancement_inspector<rl::effect> m_inspector;
		};
	}
}