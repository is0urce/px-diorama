// name: workshop_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "item_functional.hpp"

#include "drm/rl/craft/recipe.hpp"
#include "drm/rl/craft/crafting.hpp"

#include <px/common/enumerable.hpp>
#include <px/rl/loot/inventory.hpp>

#include <px/ui/board.hpp>
#include <px/ui/button.hpp>
#include <px/ui/list.hpp>
#include <px/ui/panel.hpp>
#include <px/ui/text.hpp>

#include <memory>
#include <vector>

namespace px {
	namespace ui {

		class workshop_panel
			: public panel
		{
		public:
			typedef enumerable<std::vector<rl::recipe>> recipes_type;

		public:
			void assign_container(container_component * user_container) noexcept
			{
				m_container = user_container;
				m_inventory->assign_container(user_container);
			}
			void break_links() noexcept
			{
				m_container = nullptr;
				m_inventory->detach_container();
			}
			void close_transactions()
			{
				remove_ingredients();
				recipe_clear();
			}

		public:
			virtual ~workshop_panel()
			{
			}
			workshop_panel()
				: m_container(nullptr)
				, m_inventory(nullptr)
				, m_slots(nullptr)
			{
				// recipes
				load_recipes();

				auto recipe_block = make<panel>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.33, 0.5 } });
				recipe_block->make<board>(fill, color{ 1.0, 1.0, 1.0, 0.5 });
				auto recipe_list = recipe_block->make<list<recipes_type>>(fill);
				recipe_list->assign_container(&m_recipes);
				recipe_list->set_format([](auto const& recipe) { return recipe.name; });
				recipe_list->set_text_alignment(text_alignment::right);
				recipe_list->on_click([this](auto const& recipe) {
					recipe_set(recipe);
				});

				// slots
				auto slots_block = make<panel>({ { 0.33, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.33, 0.5 } });
				slots_block->make<board>(fill, color{ 1.0, 1.0, 0.0, 0.5 });
				m_slots = slots_block->make<panel>(fill).get();

				// inventory
				auto inventory_block = make<panel>({ { 0.66, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.33, 0.5 } });
				inventory_block->make<board>(fill, color{ 1.0, 1.0, 1.0, 0.5 });
				inventory_block->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 1 },{ 1.0, 0.0 } }, "Inventory");
				m_inventory = inventory_block->make<list<rl::inventory>>({ { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 1.0, 1.0 } }).get();
				m_inventory->set_format<item_name>();
				m_inventory->on_click([this](auto & item) {
					add_ingredient(item);
				});

				// button
				auto craft_block = make<board>({ { 0.25, 0.8 },{ -1,0 },{ 1, 1 },{ 0.5, 0.0 } }, color{ 1, 0.5,0, 1 });
				auto craft_press = craft_block->make<button>(fill);
				auto craft_text = craft_block->make<text>(fill, "CRAFT");

				craft_text->set_alignment(text_alignment::center);
				craft_press->on_click([this](int /* mouse_button */) {
					craft();
				});
			}

		private:
			void load_recipes()
			{
				rl::item base_sword;
				base_sword.set_name("sword");
				base_sword.add(rl::item::enhancement_type::integer(rl::effect::equipment, static_cast<uint32_t>(rl::equipment_slot::weapon_main)));

				rl::item base_mace;
				base_mace.set_name("mace");
				base_mace.add(rl::item::enhancement_type::integer(rl::effect::equipment, static_cast<uint32_t>(rl::equipment_slot::weapon_main)));

				rl::item base_dagger;
				base_dagger.set_name("dagger");
				base_dagger.add(rl::item::enhancement_type::integer(rl::effect::equipment, static_cast<uint32_t>(rl::equipment_slot::weapon_main)));

				m_recipes.push_back({ "sword", rl::recipe_category::weapon, rl::ingredient::ore, 8, base_sword });
				m_recipes.push_back({ "mace", rl::recipe_category::weapon, rl::ingredient::ore, 6, base_mace });
				m_recipes.push_back({ "dagger", rl::recipe_category::weapon, rl::ingredient::ore, 4, base_dagger });
			}
			void recipe_set(rl::recipe const& recipe)
			{
				recipe_clear(); // clear previous

				m_current = &recipe;
				make_slots(recipe.ingredient_count); // prepare slots

				// apply filter
				switch (recipe.category)
				{
				case rl::recipe_category::weapon:
				case rl::recipe_category::armor: {
					m_inventory->set_filter([](auto const& item) {
						return item->value<rl::effect::ore_power>() != 0;
					});
					break;
				}
				default: {
					m_inventory->set_filter([](auto const& /*item*/) {
						return false;
					});
					break;
				}
				}
			}
			void recipe_clear()
			{
				m_current = nullptr;
				make_slots(0); // close slots

				// reset filter
				m_inventory->set_filter([](auto const& /*item*/) {
					return true;
				});
			}
			void craft()
			{
				size_t size = m_ingredients.size();

				std::vector<rl::item const*> ingredients;
				ingredients.assign(size, nullptr);

				bool complete = size != 0;
				for (size_t i = 0; i != size; ++i) {
					complete = complete && m_ingredients[i];
					ingredients[i] = m_ingredients[i].get();
				}

				if (m_current && complete) {

					std::shared_ptr<rl::item> craft_result = rl::crafting::make(*m_current, ingredients);

					if (craft_result) {
						m_container->add(craft_result);
						m_ingredients.assign(size, nullptr);
					}
				}
			}
			void make_slots(int total)
			{
				px_assert(m_slots);
				px_assert(total >= 0);

				remove_ingredients(); // clear unfinished
				m_ingredients.resize(total);
				m_slots->clear_anonimous(); // clear ui hierarchy

				for (int i = 0; i < total; ++i) {

					auto slot_block = m_slots->make<board>({ { 0.5, 0.0 },{ -8, 1 + i * 2 },{ 16, 1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
					auto slot_press = slot_block->make<button>(fill);

					auto txt = slot_block->make<text>(fill, [this, i]() {
						return item_name::format(m_ingredients[i]);
					});
					txt->set_alignment(text_alignment::center);
					slot_press->on_click([this, i](int /* mouse_button */) {
						remove_ingredient(i);
					});
				}
			}
			void add_ingredient(std::shared_ptr<rl::item> item)
			{
				// find first empty slot
				size_t size = m_ingredients.size();
				size_t select_index = size;
				for (size_t i = 0; i != size; ++i) {
					if (!m_ingredients[i]) {
						select_index = i;
						break;
					}
				}

				px_assert(m_container);

				if (m_container && select_index < size) {

					m_ingredients[select_index] = m_container->take(item, 1);
				}
			}
			void remove_ingredient(size_t slot_index)
			{
				px_assert(m_container);

				if (m_container) {

					auto & item = m_ingredients[slot_index];
					if (item) {
						m_container->add(item);
						m_ingredients[slot_index].reset();
					}
				}
			}
			void remove_ingredients()
			{
				size_t size = m_ingredients.size();
				for (size_t i = 0; i != size; ++i) {
					remove_ingredient(i);
				}
			}

		private:
			container_component *	m_container;
			list<rl::inventory> *	m_inventory;
			panel *					m_slots;
			std::vector<std::shared_ptr<rl::item>> m_ingredients;

			recipes_type			m_recipes;
			rl::recipe const*		m_current;
		};
	}
}