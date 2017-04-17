// name: toggle.hpp
// type: c++
// desc: class
// auth: isource

// button widget

#pragma once

#include "panel.hpp"

namespace px {
	namespace ui {

		class toggle
			: public panel
		{
		public:
			toggle() noexcept
			{
			}
			void assign_content(panel_ptr content) noexcept
			{
				m_content = content;
			}
			void assign_content(panel_ptr content, bool enabled) noexcept
			{
				assign_content(content);
				content->set_toggle(enabled);
			}
			void clear_content() noexcept
			{
				m_content.reset();
			}
			auto get_content() noexcept
			{
				return m_content;
			}

		protected:
			virtual bool click_panel(point2 const& /*position*/, int /*button*/) const override
			{
				auto content = m_content.lock();
				if (content)
				{
					content->reverse_toggle();
					return true;
				}
				return false;
			}
			//virtual rectangle layout_panel(rectangle const& parent) const override
			//{
			//	rectangle result = panel::layout_panel(parent);
			//	point2 extend{ 0, 0 };

			//	auto content = m_content.lock();
			//	if (content && content->active())
			//	{
			//		extend = { 0, content->bounds().height() };
			//	}
			//	return rectangle(result.start(), result.range().moved(extend));
			//}

		private:
			std::weak_ptr<panel> m_content;
		};
	}
}