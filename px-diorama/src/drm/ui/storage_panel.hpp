// name: storage_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

namespace px {
	namespace ui {

		class storage_panel
			: public panel
		{
		public:
			virtual ~storage_panel()
			{
			}
			storage_panel()
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
			}
			virtual bool click_panel(point2 const& /*position*/, int /*button*/) const override
			{
				return true;
			}
			virtual rectangle layout_panel(rectangle const& parent) const override
			{
			}

		private:
		};
	}
}