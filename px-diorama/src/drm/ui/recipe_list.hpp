// name: recipe_list.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/rl/recipe.hpp"

#include <list>

namespace px {
	namespace ui {

		class recipe_list
			: public panel
		{
		public:
			virtual ~recipe_list()
			{
			}
			recipe_list(std::list<recipe> list) : m_recipes(list)
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				int index = 0;
				for (auto const& recipe : m_recipes)
				{
					window.print({ 0, 0 + index }, 0xffffff, recipe.name);
					++index;
				};
			}
			virtual bool click_panel(point2 const& /*position*/, int /*button*/) const override
			{
				return true;
			}
			virtual rectangle layout_panel(rectangle const& parent) const override
			{
				rectangle base = panel::layout_panel(parent);
				int extend = static_cast<int>(m_recipes.size());
				return rectangle(base.start(), base.range().moved(0, extend));
			}

		private:
			std::list<recipe> m_recipes;
		};
	}
}