// name: panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "alignment.hpp"
#include "canvas.hpp"
#include "display.hpp"

#include <string>
#include <vector>
#include <memory>
#include <px/common/toggle.hpp>

namespace px
{
	namespace ui
	{
		class panel
			: public toggle<true>
		{
		public:
			typedef std::string name_type;

		public:
			// accessory
			name_type name() const noexcept
			{
				return m_name;
			}
			void layout(rectangle rect)
			{
				m_bounds = m_align.calculate(rect);
				m_display.reframe(m_bounds);

				panel_action([&](auto p) {
					p->layout(m_bounds);
					return false;
				});
			}
			rectangle bounds() const noexcept
			{
				return m_bounds;
			}
			point2 range() const noexcept
			{
				return m_bounds.range();
			}

			// creation
			void add(name_type name, alignment align, std::shared_ptr<panel> p)
			{
				p->m_name = name;
				p->m_align = align;
				p->m_bounds = align.calculate(m_bounds);
				m_stack[name] = p;
			}
			void add(alignment align, std::shared_ptr<panel> p)
			{
				p->m_name = "";
				p->m_align = align;
				p->m_bounds = align.calculate(m_bounds);
				m_unnamed.push_back(p);
			}
			template <typename SubPanel, typename ...Args>
			std::shared_ptr<SubPanel> make(alignment align, Args &&...args)
			{
				auto result = std::make_shared<SubPanel>(std::forward<Args>(args)...);
				add(align, result);
				return result;
			}
			template <typename SubPanel, typename ...Args>
			std::shared_ptr<SubPanel> make(name_type name, alignment align, Args &&...args)
			{
				auto result = std::make_shared<SubPanel>(std::forward<Args>(args)...);
				add(name, align, result);
				return result;
			}

			// input
			void draw(canvas & cnv)
			{
				m_display.assign(&cnv);
				draw_panel(m_display);
				panel_action([&](auto p) {
					p->draw(cnv);
					return false;
				});
			}
			template<typename Key>
			void press(Key code)
			{
				press_panel(static_cast<unsigned int>(code));
			}
			void hover(point2 position)
			{
				hover_panel(position - m_bounds.start());
			}

		protected:
			virtual void draw_panel(display & /*window*/) const
			{
			}
			virtual void press_panel(unsigned int /*code*/) const
			{
			}
			virtual void hover_panel(point2 const& /*position*/) const
			{
			}

		public:
			panel()
				: m_align(alignment::fill())
			{
			}

		private:
			template<typename Op>
			bool panel_action(Op && act)
			{
				for (auto &p : m_stack)
				{
					if (p.second && p.second->active() && std::forward<Op>(act)(p.second)) return true;
				}
				for (auto &p : m_unnamed)
				{
					if (p && p->active() && std::forward<Op>(act)(p)) return true;
				}
				return false;
			}

		private:
			std::vector<std::shared_ptr<panel>> m_unnamed;
			std::map<name_type, std::shared_ptr<panel>> m_stack;

			name_type m_name;
			alignment m_align;
			rectangle m_bounds;
			display m_display;
		};
	}
}