// name: menu.cpp

#include "menu.hpp"

#include "../key.hpp"
#include "../es/body_component.hpp"
#include "../es/container_component.hpp"

#include "inventory_panel.hpp"
#include "recipe_list.hpp"
#include "target_panel.hpp"
#include "skill_panel.hpp"

#include "item_functional.hpp"

#include <px/ui/panel.hpp>
#include <px/ui/board.hpp>
#include <px/ui/text.hpp>
#include <px/ui/button.hpp>
#include <px/ui/toggle_panel.hpp>

#include <string>
#include <memory>

namespace px {
	namespace ui {

		menu::~menu()
		{
		}
		menu::menu()
			: m_main(std::make_unique<panel>())
			, m_inventory(nullptr)
			, m_status(nullptr)
			, m_target(nullptr)
			, m_container(nullptr)
			, m_inspector(nullptr)
		{
			initialize();
		}

		panel * menu::main() noexcept
		{
			return m_main.get();
		}
		panel const* menu::main() const noexcept
		{
			return m_main.get();
		}
		void menu::assign_incarnation(transform_component * pawn)
		{
			m_transform = pawn;
			m_body = pawn ? pawn->linked<body_component>() : nullptr;
			m_storage = m_body ? m_body->linked<container_component>() : nullptr;

			m_status->lock_target(pawn);
		}
		void menu::assign_target(transform_component * pawn, point2 location)
		{
			m_target->lock(pawn, location);
		}
		void menu::break_links()
		{
			m_inventory->break_links();
			m_container->assign_container(nullptr);
			m_inspector->assign_container(nullptr);
			m_target->clear_target();
			m_status->clear_target();
		}
		void menu::close_transactions()
		{
			close_sheets();
		}
		void menu::open_storage(container_component * storage)
		{
			open_storage(storage, m_storage);
		}
		void menu::open_storage(container_component * storage, container_component * inspector)
		{
			close_sheets();

			m_container->assign_container(storage);
			m_inspector->assign_container(inspector);

			(*m_main)["container_access"]->activate();
		}
		void menu::open_workshop(container_component * /* user */)
		{
			close_sheets();
		}
		void menu::close_sheets()
		{
			px_assert(m_inventory);

			m_inventory->deactivate();
			(*m_main)["container_access"]->deactivate();
		}
		void menu::toggle_inventory()
		{
			px_assert(m_inventory);

			bool opened = m_inventory->active();

			close_sheets();

			m_inventory->assign_container(m_body, m_storage);
			m_inventory->activate(!opened);
		}

		void menu::initialize()
		{
			// status and target panel
			auto status = m_main->make<target_panel>({ { 0.0, 1.0 },{ 1, -5 },{ -2, 4 },{ 0.5, 0.0 } });
			auto target = m_main->make<target_panel>({ { 0.5, 1.0 },{ 1, -5 },{ -2, 4 },{ 0.5, 0.0 } });

			m_main->make<skill_panel>({ { 0.0, 1.0 },{ 1, -2 },{ -2, 1 },{ 1.0, 0.0 } }, status.get(), target.get());
			m_inventory = m_main->make<inventory_panel>({ { 0.25, 0.2 },{ 0, 0 },{ 0, 0 },{ 0.5, 0.6 } }).get(); // inventory_panel

			// inspect container panel block
			auto container_block = m_main->make<panel>("container_access", { { 0.25, 0.0 },{ 0, 1 },{ 0, -2 },{ 0.5, 1.0 } });
			container_block->make<board>("background", fill, color{ 1, 1, 1, 0.75 });
			auto container_list = container_block->make<inventory_list>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } });
			auto inspector_list = container_block->make<inventory_list>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } });

			container_list->on_click(item_transfer<inventory_list*>(container_list.get(), inspector_list.get()));
			inspector_list->on_click(item_transfer<inventory_list*>(inspector_list.get(), container_list.get()));
			//container_list->set_filter([](auto const& item) { return item->has_effect<rl::effect::ore_power>(); });
			//inspector_list->set_filter([](auto const& item) { return item->has_effect<rl::effect::ore_power>(); });
			container_list->set_format<item_name>();
			inspector_list->set_format<item_name>();

			// inventory button
			auto i_block = m_main->make<board>({ { 0.0, 0.0 },{ 1,1 },{ 1,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
			i_block->make<text>(fill, "i");
			auto i_button = i_block->make<button>(fill);
			i_button->register_shortcut(static_cast<unsigned int>(key::panel_inventory));
			i_button->on_click([this](int /* mouse_button */) { toggle_inventory(); });

			//auto inventory_toggle = m_ui.make<ui::toggle_panel>("inventory_toggle", { {0.25, 0.25}, {0, 0}, {0, 1}, {0.5, 0.0} });
			//inventory_toggle->add_background({ 0, 0, 0.5, 1 });
			//inventory_toggle->add_label("Inventory");
			//inventory_toggle->assign_content(inventory_block, false);

			//std::list<recipe> recipes;
			//recipes.push_back({ "sword", recipe_type::weapon, 8 });
			//recipes.push_back({ "mace", recipe_type::weapon, 6});
			//recipes.push_back({ "dagger", recipe_type::weapon, 4 });
			//m_ui.make<ui::recipe_list>("recipes", { {0.0, 0.0}, {0,0}, {0,0}, {0.5,0.0} }, std::move(recipes));

			// store typed links
			m_status = status.get();
			m_target = target.get();
			m_container = container_list.get();
			m_inspector = inspector_list.get();

			// end setup
			close_sheets();
		}
	}
}