// name: toggle.hpp
// type: c++
// desc: class
// auth: isource

// button widget

#pragma once

#include "panel.hpp"

#include "board.hpp"
#include "text.hpp"

namespace px {
	namespace ui {

		class toggle_panel
			: public panel
		{
		public:
			void assign_content(panel_ptr content) noexcept
			{
				m_content = content;
			}
			void assign_content(panel_ptr content, bool enabled) noexcept
			{
				assign_content(content);
				content->activate(enabled);
			}
			void clear_content() noexcept
			{
				m_content.reset();
			}
			auto get_content() noexcept
			{
				return m_content;
			}
			bool content_active() const noexcept
			{
				auto content = m_content.lock();
				return content && content->active();
			}
			void add_background(color bg_color)
			{
				make<ui::board>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 1 },{ 1.0, 0.0 } }, bg_color);
			}
			void add_label(std::string closed, std::string opened)
			{
				make<ui::text>(ui::fill, [this,opened,closed]() { return content_active() ? opened : closed; });
			}
			void add_label(std::string label)
			{
				add_label("+ " + label, "- " + label);
			}

		public:
			toggle_panel() noexcept
			{
			}
			virtual ~toggle_panel()
			{
			}

		protected:
			virtual bool click_panel(point2 const& position, int /*button*/) override
			{
				auto content = m_content.lock();
				if (content && position.y() == 0)
				{
					content->reverse_toggle();
					return true;
				}
				return false;
			}
			virtual rectangle layout_panel(rectangle const& parent) const override
			{
				rectangle base = panel::layout_panel(parent);
				auto content = m_content.lock();
				int extend = (content && content->active()) ? content->bounds().height() : 0;
				return rectangle(base.start(), base.range().moved(0, extend));
			}

		private:
			std::weak_ptr<panel> m_content;
		};
	}
}