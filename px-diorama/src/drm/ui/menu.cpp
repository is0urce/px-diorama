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

#include "drm/key.hpp"

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
		void menu::open_inventory(container_component * /* inventory */)
		{
			close_sheets();
		}
		void menu::open_storage(container_component * storage, container_component * user)
		{
			close_sheets();

			m_container->assign_container(storage);
			m_inventory->assign_container(user);

			(*m_main)["container_access"].activate();
		}
		void menu::open_workshop(container_component * /* user */)
		{
			close_sheets();
		}
		void menu::close_sheets()
		{
			m_container->assign_container(nullptr);
			m_inventory->assign_container(nullptr);

			(*m_main)["container_access"].deactivate();
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

			// target status
			auto target = m_main->make<target_panel>("target", { { 0.0, 1.0 },{ 1, -2 },{ -2, 1 },{ 1.0, 0.0 } });
			m_target = target.get();

			// storage block
			auto container = m_main->make<panel>("container_access", { { 0.25, 0.0 },{ 0, 1 },{ 0, -2 },{ 0.5, 1.0 } });
			container->make<board>(ui::fill, color{ 1, 1, 1, 0.75 });
			auto container_inventory = container->make<ui::inventory_list>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } });
			auto user_inventory = container->make<ui::inventory_list>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } });

			m_container = container_inventory.get();
			m_inventory = user_inventory.get();

			m_container->set_format([](auto const& item) { return item->name(); });
			m_inventory->set_format([](auto const& item) { return item->name(); });
			m_container->on_click([=](auto & item) {
				auto from = m_container->list();
				auto to = m_inventory->list();
				if (from && to) from->transfer(item, *to);
			});
			m_inventory->on_click([=](auto & item) {
				auto from = m_inventory->list();
				auto to = m_container->list();
				if (from && to) from->transfer(item, *to);
			});

			// inventory button
			auto i_block = m_main->make<board>({ { 0.0, 0.0 },{ 1,1 },{ 1,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
			i_block->make<text>(ui::fill, "i");
			auto i_button = i_block->make<button>(ui::fill);
			i_button->on_click([&](int /* mouse_button */) { close_sheets(); });
			i_button->register_shortcut(static_cast<unsigned int>(key::panel_inventory));

			// setup
			close_sheets();
		}
	}
}