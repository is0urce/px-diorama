// name: panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "alignment.hpp"
#include "canvas.hpp"
#include "display.hpp"

#include <px/common/toggle.hpp>

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>

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
			rectangle bounds() const noexcept
			{
				return m_bounds;
			}
			point2 range() const noexcept
			{
				return m_bounds.range();
			}
			void layout(rectangle rect) noexcept
			{
				m_bounds = calculate_alignment(m_align, rect);
				m_display.reframe(m_bounds);

				panel_action([&](auto &subpanel) {
					subpanel->layout(m_bounds);
					return false;
				});
			}

			// creation
			void add(name_type name, alignment align, std::shared_ptr<panel> child)
			{
				if (!child) throw std::runtime_error("px::ui::panel::add(name, align, child) - child is null");

				child->m_name = name;
				child->m_align = align;
				child->m_bounds = calculate_alignment(align, m_bounds);
				m_stack[name] = child;
			}
			void add(alignment align, std::shared_ptr<panel> child)
			{
				if (!child) throw std::runtime_error("px::ui::panel::add(align, child) - child is null");

				child->m_name = "";
				child->m_align = align;
				child->m_bounds = calculate_alignment(align, m_bounds);
				m_unnamed.push_back(child);
			}
			void remove_all() noexcept
			{
				remove_all_anonimous();
				remove_all_tagged();
			}
			void remove_all_anonimous() noexcept
			{
				m_unnamed.clear();
			}
			void remove_all_tagged() noexcept
			{
				m_stack.clear();
			}
			void remove(name_type const& name)
			{
				m_stack.erase(name);
			}
			void remove(panel const* ptr)
			{
				m_unnamed.erase(std::remove_if(std::begin(m_unnamed), std::end(m_unnamed),
					[ptr](auto const& subpanel) { return subpanel.get() == ptr; }),
					std::end(m_unnamed));
			}
			template <typename SubPanel, typename ...Args>
			auto make(alignment align, Args &&... args)
			{
				auto result = std::make_shared<SubPanel>(std::forward<Args>(args)...);
				add(align, result);
				return result;
			}
			template <typename SubPanel, typename ...Args>
			auto make(name_type name, alignment align, Args &&... args)
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
			panel() noexcept
				: m_align({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } })
			{
			}

		private:
			template<typename Operator>
			bool panel_action(Operator && callback_action)
			{
				for (auto & p : m_stack)
				{
					if (p.second && p.second->active() && std::forward<Operator>(callback_action)(p.second)) return true;
				}
				for (auto & p : m_unnamed)
				{
					if (p && p->active() && std::forward<Operator>(callback_action)(p)) return true;
				}
				return false;
			}
			static rectangle calculate_alignment(alignment const& align, rectangle const& parent) noexcept
			{
				point2 start = align.anchor_offset + parent.start() + (align.anchor_percent * parent.range()).ceil();
				point2 range = align.size_absolute + (align.size_relative * parent.range()).ceil();
				return{ start, range };
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