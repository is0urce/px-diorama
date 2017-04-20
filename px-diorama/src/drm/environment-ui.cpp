#include "environment.hpp"

#include <px/ui/board.hpp>
#include <px/ui/text.hpp>
#include <px/ui/button.hpp>
#include <px/ui/toggle_panel.hpp>

#include "ui/inventory_list.hpp"
#include "ui/recipe_list.hpp"
#include "ui/target_panel.hpp"

namespace px {

	ui::panel & environment::ui() noexcept
	{
		return m_ui;
	}
	ui::panel const& environment::ui() const noexcept
	{
		return m_ui;
	}
	void environment::layout_ui(rectangle bounds) noexcept
	{
		m_ui.layout(bounds);
	}

	void environment::expose_inventory(container_component * /*inventory*/)
	{
		m_ui["storage"].reverse_toggle();
	}

	void environment::setup_ui()
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

		auto storage = m_ui.make<ui::panel>("storage", { { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 0.5, 0.5 } });
		storage->make<ui::board>("background", ui::fill, color{ 1, 1, 0, 0.5 });
		storage->deactivate();

		m_target_panel = m_ui.make<ui::target_panel>("target", { { 1.0, 1.0 },{ -26, -2 },{ 25, 1 },{ 0.0, 0.0 } });

	}
}