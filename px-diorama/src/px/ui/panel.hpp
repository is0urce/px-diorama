// name: panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "alignment.hpp"
#include "display.hpp"

#include <px/common/toggle.hpp>

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>

namespace px {
	namespace ui {

		class panel
			: public px::toggle<true>
		{
		public:
			typedef std::string name_type;
			typedef std::shared_ptr<panel> panel_ptr;

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
			alignment align() const noexcept
			{
				return m_align;
			}
			void layout(rectangle const& parent_bounds)
			{
				m_bounds = layout_panel(parent_bounds);

				// process to subpanels
				action([this](auto & subpanel) {
					subpanel->layout(m_bounds);
				});
			}
			void layout()
			{
				if (m_parent) {
					layout(m_parent->m_bounds);
				}
			}
			bool focused() const noexcept
			{
				return m_focus;
			}

			// creation
			void add(name_type name, panel_ptr child)
			{
				if (!child) throw std::runtime_error("px::ui::panel::add(name, child) - child is null");

				child->m_name = name;
				child->m_parent = this;

				child->layout(m_bounds);

				m_stack[name] = child;
			}
			void add(panel_ptr child)
			{
				if (!child) throw std::runtime_error("px::ui::panel::add(child) - child is null");

				child->m_parent = this;

				child->layout(m_bounds);

				m_unnamed.push_back(child);
			}
			void clear() noexcept
			{
				clear_anonimous();
				clear_tagged();
			}
			void clear_anonimous() noexcept
			{
				m_unnamed.clear();
			}
			void clear_tagged() noexcept
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

			panel_ptr & at(name_type const& tag)
			{
				return m_stack.at(tag);
			}
			panel_ptr const& at(name_type const& tag) const
			{
				return m_stack.at(tag);
			}
			template <typename SubPanel, typename ...Args>
			auto make(alignment align, Args &&... args)
			{
				auto result = std::make_shared<SubPanel>(std::forward<Args>(args)...);
				result->m_align = align;
				add(result);
				return result;
			}
			template <typename SubPanel, typename ...Args>
			auto make(name_type name, alignment align, Args &&... args)
			{
				auto result = std::make_shared<SubPanel>(std::forward<Args>(args)...);
				result->m_align = align;
				add(name, result);
				return result;
			}
			void set_alignment(alignment align) noexcept
			{
				m_align = align;
				layout();
			}
			void inflate(int right, int bottom, int left, int top) noexcept
			{
				m_align.size_absolute = m_align.size_absolute.moved(right, bottom);
				m_align.anchor_offset = m_align.anchor_offset.moved(-left, -top);
				layout();
			}
			void deflate(int right, int bottom, int left, int top) noexcept
			{
				inflate(-right, -bottom, -left, -top);
			}

			// input
			void draw(canvas & cnv) const
			{
				display window(&cnv, m_bounds);
				draw_panel(window);

				action([&](auto const& subpanel) {
					subpanel->draw(cnv);
				});
			}
			bool press(unsigned int code)
			{
				bool processed = false;

				// childrens
				action([&](auto & subpanel) {
					processed |= subpanel->press(code);
				});

				// this one
				return processed || press_panel(code);
			}
			bool hover(point2 absolute)
			{
				m_focus = m_bounds.contains(absolute);

				bool processed = false;

				if (m_focus) {

					point2 relative = absolute - m_bounds.start();

					// childrens
					action([&](auto & subpanel) {
						processed |= subpanel->hover(absolute); // call recursive
					});

					// this one
					if (!processed) {
						processed = hover_panel(relative); // call this virtual overload
					}
				}

				return processed;
			}

			// returns true if event dispatched by subpanels
			bool click(point2 const& position, int button)
			{
				bool processed = false;

				// childrens
				action([&](auto & subpanel) {
					processed |= subpanel->click(position, button); // call recursive
				});

				// this one
				if (!processed && m_bounds.contains(position)) {
					processed = click_panel(position - m_bounds.start(), button); // call this virtual overload
				}

				return processed;
			}

			bool scroll(double horisontal, double vertical)
			{
				bool processed = false;

				if (m_focus) {

					// childrens
					action([&](auto & subpanel) {
						processed |= subpanel->scroll(horisontal, vertical); // call recursive
					});

					// this one
					if (!processed && focused()) {
						processed = scroll_panel(horisontal, vertical); // call this virtual overload
					}
				}

				return processed;
			}

			panel_ptr & operator[](name_type const& tag)
			{
				return m_stack[tag];
			}

		public:
			virtual ~panel()
			{
			}
			panel() noexcept
				: panel("", { { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } })
			{
			}
			panel(name_type tag)
				: panel(tag, { { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } })
			{
			}
			panel(name_type tag, alignment align)
				: m_name(tag)
				, m_align(align)
				, m_parent(nullptr)
				, m_focus(false)
			{
			}
			panel(panel const&) = delete;
			panel & operator=(panel const&) = delete;

		protected:
			virtual bool hover_panel(point2 const& /* position */)
			{
				return false;
			}
			virtual bool click_panel(point2 const& /* position */, int /* button */)
			{
				return false;
			}
			virtual bool press_panel(unsigned int /* code */)
			{
				return false;
			}
			virtual bool scroll_panel(double /* horisontal */, double /* vertical */)
			{
				return false;
			}
			virtual void draw_panel(display & /* window */) const
			{
			}
			virtual rectangle layout_panel(rectangle const& parent) const
			{
				return calculate_bounds(parent);
			}

		private:
			template<typename Operator>
			bool action(Operator && callback_action)
			{
				for (auto & pair : m_stack) {
					if (pair.second && pair.second->active()) {
						callback_action(pair.second);
					}
				}
				for (auto & subpanel : m_unnamed) {
					if (subpanel && subpanel->active()) {
						callback_action(subpanel);
					}
				}
				return false;
			}
			template<typename Operator>
			bool action(Operator && callback_action) const
			{
				for (auto const& pair : m_stack) {
					if (pair.second && pair.second->active()) {
						callback_action(pair.second);
					}
				}
				for (auto const& subpanel : m_unnamed) {
					if (subpanel && subpanel->active()) {
						callback_action(subpanel);
					}
				}
				return false;
			}
			static rectangle calculate_bounds(alignment const& align, rectangle const& parent) noexcept
			{
				point2 start = align.anchor_offset + parent.start() + (align.anchor_percent * parent.range()).ceil();
				point2 range = align.size_absolute + (align.size_relative * parent.range()).ceil();
				return{ start, range };
			}
			rectangle calculate_bounds(rectangle const& parent) const noexcept
			{
				return calculate_bounds(m_align, parent);
			}

		private:
			name_type	m_name;
			alignment	m_align;
			rectangle	m_bounds;
			panel *		m_parent;

			bool		m_focus;

			std::vector<std::shared_ptr<panel>> m_unnamed;
			std::map<name_type, std::shared_ptr<panel>> m_stack;
		};
	}
}