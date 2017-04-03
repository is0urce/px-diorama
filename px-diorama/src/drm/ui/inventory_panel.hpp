// name: inventory_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/container_component.hpp"

namespace px
{
	namespace ui
	{
		class inventory_panel
			: public panel
		{
		protected:
			virtual void draw_panel(display & window) const
			{
				window.paint(0x000000);
			}
			virtual void press_panel(unsigned int /*code*/) const
			{
			}
			virtual void hover_panel(point2 const& /*position*/) const
			{
			}

		public:
			inventory_panel()
			{
			}

		private:
			container_component * m_inventory;
		};
	}
}