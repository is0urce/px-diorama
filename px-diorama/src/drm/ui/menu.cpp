// name: menu.cpp

#include "menu.hpp"

#include <px/ui/panel.hpp>

#include <px/ui/board.hpp>
#include <px/ui/text.hpp>
#include <px/ui/button.hpp>
#include <px/ui/toggle_panel.hpp>

#include "inventory_list.hpp"
#include "recipe_list.hpp"
#include "target_panel.hpp"

namespace px {
	namespace ui {

		menu::~menu()
		{
		}

		menu::menu()
			: m_main(std::make_unique<panel>())
		{
			initialize();
		}

		ui::panel * menu::main() noexcept
		{
			return m_main.get();
		}
		ui::panel const* menu::main() const noexcept
		{
			return m_main.get();
		}
		void menu::expose_inventory(container_component * /*inventory*/)
		{
			(*m_main)["storage"].reverse_toggle();
		}
		void menu::lock_target(point2 absolute, transform_component const* pawn)
		{
			m_target->lock(absolute, pawn);
		}

		void menu::initialize()
		{
			//auto inventory_block = m_ui.make<ui::panel>("inventory_block", { {0.25, 0.25}, {0, 1}, {0, -1}, {0.5, 0.5} });
			//inventory_block->make<ui::board>("background", ui::fill, color{ 0, 0, 1, 1 });
			//m_inventory = inventory_block->make<ui::inventory_list>("list", ui::fill).get();

			//auto inventory_toggle = m_ui.make<ui::toggle_panel>("inventory_toggle", { {0.25, 0.25}, {0, 0}, {0, 1}, {0.5, 0.0} });
			//inventory_toggle->add_background({ 0, 0, 0.5, 1 });
			//inventory_toggle->add_label("Inventory");
			//inventory_toggle->assign_content(inventory_block, false);

			//std::list<recipe> recipes;
			//recipes.push_back({ "sword", recipe_type::weapon, 8 });
			//recipes.push_back({ "mace", recipe_type::weapon, 6});
			//recipes.push_back({ "dagger", recipe_type::weapon, 4 });
			//m_ui.make<ui::recipe_list>("recipes", { {0.0, 0.0}, {0,0}, {0,0}, {0.5,0.0} }, std::move(recipes));

			auto storage = m_main->make<panel>("storage", { { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 0.5, 0.5 } });
			storage->make<board>("background", fill, color{ 1, 1, 0, 0.5 });
			storage->deactivate();

			auto target = m_main->make<target_panel>("target", { { 1.0, 1.0 },{ -26, -2 },{ 25, 1 },{ 0.0, 0.0 } });
			m_target = target.get();
		}
	}
}