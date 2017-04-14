// name: inventory_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/container_component.hpp"

namespace px {
	namespace ui {

		class inventory_panel
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
			inventory_panel()
			{
			}

		protected:
			virtual void draw_panel(display & window) const
			{
				if (m_container)
				{
					int x = 0;
					m_container->enumerate([&](auto const& item) {
						window.print({ 0, x }, 0xffffff, item->name());
						++x;
					});
				}
			}
			//virtual void press_panel(unsigned int /*code*/) const
			//{
			//}
			//virtual void hover_panel(point2 const& /*position*/) const
			//{
			//}

		private:
			container_component * m_container;
		};
	}
}