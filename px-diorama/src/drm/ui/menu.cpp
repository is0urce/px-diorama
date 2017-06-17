// name: menu.cpp

#include "menu.hpp"

#include "../key.hpp"
#include "../es/body_component.hpp"

#include "inventory_panel.hpp"
#include "skill_panel.hpp"
#include "storage_panel.hpp"
#include "target_panel.hpp"
#include "workshop_panel.hpp"
#include "performance_panel.hpp"

#include "item_functional.hpp"

#include <px/ui/board.hpp>
#include <px/ui/button.hpp>
#include <px/ui/panel.hpp>
#include <px/ui/text.hpp>
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

			, m_transform(nullptr)
			, m_body(nullptr)
			, m_container(nullptr)
			, m_character(nullptr)

			, m_status(nullptr)
			, m_target(nullptr)
			, m_inventory(nullptr)
			, m_storage(nullptr)
		{
			initialize();
		}

		panel * menu::main() px_noexcept
		{
			px_assert(m_main);
			return m_main.get();
		}
		panel const* menu::main() const px_noexcept
		{
			px_assert(m_main);
			return m_main.get();
		}
		void menu::assign_incarnation(transform_component * pawn) noexcept
		{
			m_transform = pawn;
			m_body = pawn ? pawn->linked<body_component>() : nullptr;
			m_container = m_body ? m_body->linked<container_component>() : nullptr;

			px_assert(m_status);

			m_status->lock_target(pawn);
		}
		void menu::assign_target(transform_component * pawn, point2 location) noexcept
		{
			px_assert(m_target);

			m_target->lock(pawn, location);
		}
		void menu::break_links() noexcept
		{
			m_inventory->break_links();
			m_storage->break_links();
			m_workshop->break_links();
			m_target->clear_target();
			m_status->clear_target();
		}
		bool menu::close_transactions()
		{
			m_storage->deactivate();
			m_workshop->close_transactions();
			return true;
		}
		void menu::open_storage(container_component * storage)
		{
			open_storage(storage, m_container);
		}
		void menu::open_storage(container_component * storage, container_component * inspector)
		{
			px_assert(m_storage);

			close_sheets();

			m_storage->assign_containers(storage, inspector);
			m_storage->activate();
		}
		void menu::open_workshop(container_component * user)
		{
			close_sheets();

			px_assert(m_workshop);

			m_workshop->assign_container(user);
			m_workshop->activate();
		}
		void menu::close_sheets()
		{
			px_assert(m_inventory);
			px_assert(m_storage);
			px_assert(m_workshop);

			m_inventory->deactivate();
			m_storage->deactivate();
			m_workshop->deactivate();
		}
		void menu::toggle_inventory()
		{
			px_assert(m_inventory);

			bool opened = m_inventory->active();
			close_sheets();

			m_inventory->assign_container(m_body, m_container);
			m_inventory->activate(!opened);
		}

		void menu::initialize()
		{
			m_main->make<performance_panel>(fill);

			// status info
			m_status = m_main->make<target_panel>({ { 0.0, 1.0 },{ 1, -5 },{ -2, 4 },{ 0.5, 0.0 } }).get(); // status and target panel
			m_target = m_main->make<target_panel>({ { 0.5, 1.0 },{ 1, -5 },{ -2, 4 },{ 0.5, 0.0 } }).get();
			m_main->make<skill_panel>({ { 0.0, 1.0 },{ 1, -2 },{ -2, 1 },{ 1.0, 0.0 } }, m_status, m_target); // skills panel

			// sheets
			m_inventory = m_main->make<inventory_panel>({ { 0.25, 0.2 },{ 0, 0 },{ 0, 0 },{ 0.5, 0.6 } }).get(); // inventory_panel
			m_storage = m_main->make<storage_panel>({ { 0.25, 0.0 },{ 0, 1 },{ 0, -2 },{ 0.5, 1.0 } }).get();
			m_workshop = m_main->make<workshop_panel>(fill).get();

			// inventory button
			auto i_block = m_main->make<board>({ { 0.0, 0.0 },{ 1,1 },{ 1,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
			i_block->make<text>(fill, "i");
			auto i_button = i_block->make<button>(fill);
			i_button->register_shortcut(static_cast<unsigned int>(key::panel_inventory));
			i_button->on_click([this](int /* mouse_button */) { toggle_inventory(); });

			// craft button
			auto k_block = m_main->make<board>({ { 0.0, 0.0 },{ 2,1 },{ 1,1 },{ 0, 0 } }, color{ 1, 0.5,0, 1 });
			k_block->make<text>(fill, "k");
			auto k_button = k_block->make<button>(fill);
			k_button->on_click([this](int /* mouse_button */) { open_workshop(m_container); });

			// end setup
			close_sheets();
		}
	}
}