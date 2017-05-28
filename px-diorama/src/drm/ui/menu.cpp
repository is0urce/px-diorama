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

#include <string>

namespace px {
	namespace ui {

		namespace {
			template <typename Container>
			class item_transfer
			{
			public:
				template <typename Item>
				void operator()(Item & item)
				{
					auto * from = source->list();
					auto * to = destination->list();
					if (from && to) from->transfer(item, *to);
				}
				item_transfer(Container * from, Container * to)
					: source(from)
					, destination(to)
				{
				}
				Container * source;
				Container * destination;
			};
			struct item_name
			{
				template <typename Item>
				std::string operator()(Item & item)
				{
					std::string result = item->name();
					result += " x" + std::to_string(item->count());
					return result;
				}
			};
		}

		menu::~menu()
		{
		}
		menu::menu()
			: m_main(std::make_unique<panel>())
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
		void menu::break_links()
		{
			m_container->assign_container(nullptr);
			m_inspector->assign_container(nullptr);
			m_inventory->assign_container(nullptr);
		}
		void menu::assign_inventory(container_component * user)
		{
			m_player = user;
		}
		void menu::open_inventory(container_component * inventory)
		{
			close_sheets();

			m_inventory->assign_container(inventory);

			(*m_main)["inventory_access"].activate();
		}
		void menu::open_storage(container_component * storage, container_component * inspector)
		{
			close_sheets();

			m_container->assign_container(storage);
			m_inspector->assign_container(inspector);

			(*m_main)["container_access"].activate();
		}
		void menu::open_workshop(container_component * /* user */)
		{
			close_sheets();
		}
		void menu::close_sheets()
		{
			break_links();
			(*m_main)["container_access"].deactivate();
			(*m_main)["inventory_access"].deactivate();
		}
		void menu::lock_target(point2 absolute, transform_component const* pawn)
		{
			m_target->lock(absolute, pawn);
		}

		void menu::initialize()
		{
			auto inventory = m_main->make<panel>("inventory_access", { {0.0, 0.0}, {0, 0}, {0, 0}, {0.5, 0.5} });
			auto player_inventory = inventory->make<inventory_list>(fill);
			m_inventory = player_inventory.get();
			m_inventory->set_format(item_name{});
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
			auto target = m_main->make<target_panel>("target", { { 0.0, 1.0 },{ 1, -5 },{ -2, 4 },{ 1.0, 0.0 } });
			m_target = target.get();

			// storage block
			auto container = m_main->make<panel>("container_access", { { 0.25, 0.0 },{ 0, 1 },{ 0, -2 },{ 0.5, 1.0 } });
			container->make<board>(fill, color{ 1, 1, 1, 0.75 });
			auto container_inventory = container->make<inventory_list>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 0.5, 1.0 } });
			auto user_inventory = container->make<inventory_list>({ { 0.5, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } });

			m_container = container_inventory.get();
			m_inspector = user_inventory.get();

			m_container->set_format(item_name{});
			m_inspector->set_format(item_name{});
			m_container->on_click(item_transfer<inventory_list>(m_container, m_inspector));
			m_inspector->on_click(item_transfer<inventory_list>(m_inspector, m_container));
			m_container->set_filter([](auto const& item) { return item->has_effect<rl::effect::ore_power>(); });
			m_inspector->set_filter([](auto const& item) { return item->has_effect<rl::effect::ore_power>(); });

			// inventory button
			auto i_block = m_main->make<board>({ { 0.0, 0.0 },{ 1,1 },{ 1,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
			i_block->make<text>(fill, "i");
			auto i_button = i_block->make<button>(fill);
			i_button->register_shortcut(static_cast<unsigned int>(key::panel_inventory));
			i_button->on_click([&](int /* mouse_button */) { open_inventory(m_player); });

			// setup
			close_sheets();
		}
	}
}