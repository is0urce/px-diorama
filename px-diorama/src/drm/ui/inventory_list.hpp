// name: inventory_list.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/container_component.hpp"

namespace px {
	namespace ui {

		class inventory_list
			: public panel
		{
		public:
			void set_container(container_component * container) noexcept
			{
				m_container = container;
			}
			void clear_container() noexcept
			{
				m_container = nullptr;
			}

		public:
			inventory_list()
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
			//virtual void press_panel(unsigned int /*code*/) const
			//{
			//}
			//virtual void hover_panel(point2 const& /*position*/) const
			//{
			//}
			virtual bool click_panel(point2 const& position, int /*button*/) const override
			{
				if (m_container)
				{
					int index = 0;
					m_container->enumerate([&](auto const& item) {
						++index;
					});
				}
				return true;
			}

		private:
			container_component * m_container;
		};
	}
}