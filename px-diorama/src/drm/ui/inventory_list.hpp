// name: inventory_list.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/container_component.hpp"

#include <functional>

namespace px {
	namespace ui {

		class inventory_list
			: public panel
		{
		public:
			typedef std::function<void(std::shared_ptr<rl::item> & pos)> click_fn;

		public:
			void assign_container(container_component * container) noexcept
			{
				m_container = container;
			}
			void on_click(click_fn click_action)
			{
				m_click = click_action;
			}
			void clear_container() noexcept
			{
				m_container = nullptr;
			}

		public:
			virtual ~inventory_list()
			{
			}
			inventory_list()
				: m_container(nullptr)
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				if (m_container)
				{
					int index = 0;
					m_container->enumerate([&](auto const& item) {
						window.print({ 0, 0 + index }, 0xffffff, item->name());
						++index;
					});
				}
			}
			virtual bool click_panel(point2 const& position, int /*button*/) override
			{
				if (m_container && m_click)
				{
					int selected = position.y();
					int index = 0;
					m_container->enumerate_while([&](auto & item) {
						if (selected == index) {
							m_click(item);

							return false;
						}

						++index;

						return true;
					});
				}
				return true;
			}

		private:
			container_component * m_container;

			click_fn m_click;
		};
	}
}