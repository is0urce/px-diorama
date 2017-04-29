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
		void menu::open_storage(container_component * storage_inventory, container_component * user_inventory)
		{
			close_panels();

			if (storage_inventory && user_inventory)
			{
				m_container->assign_container(storage_inventory);
				m_inventory->assign_container(user_inventory);

				m_container->on_click([storage_inventory, user_inventory](auto & item) {
					storage_inventory->transfer(item, *user_inventory);
				});
				m_inventory->on_click([storage_inventory, user_inventory](auto & item) {
					user_inventory->transfer(item, *storage_inventory);
				});

				(*m_main)["container_access"].activate();
			}
		}
		void menu::close_storage()
		{
			m_container->assign_container(nullptr);
			m_inventory->assign_container(nullptr);

			m_container->on_click([](auto & /*item*/) { });
			m_inventory->on_click([](auto & /*item*/) { });

			(*m_main)["container_access"].deactivate();
		}
		void menu::close_panels()
		{
			close_storage();
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

			auto target = m_main->make<target_panel>("target", { { 0.5, 1.0 },{ -1, -2 },{ 0, 1 },{ 0.5, 0.0 } });
			m_target = target.get();

			auto container = m_main->make<panel>("container_access", { { 0.0, 0.0 },{ 1, 1 },{ -2, -2 },{ 1.0, 1.0 } });
			container->make<board>("bg", fill, color{ 1, 1, 1,0.5 });
			auto container_inventory = container->make<ui::inventory_list>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } });
			auto user_inventory = container->make<ui::inventory_list>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } });

			container_inventory->set_format([](auto const& item) { return item->name(); });
			user_inventory->set_format([](auto const& item) { return item->name(); });

			m_container = container_inventory.get();
			m_inventory = user_inventory.get();

			close_storage();
		}
	}
}