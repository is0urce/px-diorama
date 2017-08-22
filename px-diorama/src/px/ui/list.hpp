// name: list.hpp
// type: c++
// desc: template class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>
#include <px/ui/text_alignment.hpp>

#include <functional>
#include <stdexcept>
#include <typeinfo>

namespace px {
	namespace ui {

		template <typename Container>
		class list
			: public panel
		{
		public:
			typedef Container container_type;
			typedef typename Container::value_type element_type;
			typedef std::function<void(element_type &)> event_fn;
			typedef std::function<std::string(element_type const&)> format_fn;
			typedef std::function<bool(element_type const&)> filter_fn;

		public:
			void assign_container(container_type * container) noexcept
			{
				m_container = container;
				m_scroll = 0;
			}
			void detach_container() noexcept
			{
				m_container = nullptr;
				m_scroll = 0;
			}

			template <typename Format>
			void set_format()
			{
				m_format = Format{};
			}
			void set_format(format_fn format)
			{
				if (!format) throw std::invalid_argument("px::inventory_list::set_format(format) - format is null");
				m_format = format;
			}
			void set_filter(filter_fn filter)
			{
				if (!filter) throw std::invalid_argument("px::inventory_list::set_filter(filter) - filter is null");
				m_filter = filter;
			}
			void set_color(color text_color)
			{
				m_color = text_color;
			}
			void on_click(event_fn click_action)
			{
				m_click = click_action;
			}
			void on_hover(event_fn hover_action)
			{
				m_hover = hover_action;
			}
			container_type * assigned_container()
			{
				return m_container;
			}
			void set_text_alignment(text_alignment align)
			{
				m_text_alignment = align;
			}

		public:
			virtual ~list()
			{
			}
			list()
				: m_container(nullptr)
				, m_color(0x000000)
				, m_text_alignment(text_alignment::left)
				, m_scroll(0)
				, m_format([](auto const&) { return typeid(element_type).name(); })
				, m_filter([](auto const&) { return true; })
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				if (m_container && m_format)
				{
					int index = 0 - m_scroll;

					int write_width = bounds().width();
					m_container->enumerate([&](auto const& item) {
						if (m_filter(item)) {

							auto str = m_format(item);
							int text_length = static_cast<int>(str.length());


							int text_offset = 0;
							switch (m_text_alignment) {
							case text_alignment::center:
								text_offset = (write_width - text_length) / 2;
								break;
							case text_alignment::right:
								text_offset = write_width - text_length;
								break;
							}

							window.print({ text_offset, 0 + index }, m_color, str);
							++index;
						}
					});
				}
			}
			virtual bool click_panel(point2 const& position, int mouse_button) override
			{
				if (m_click && mouse_button == 0) {
					element_type * found = find(position.y());

					if (found) m_click(*found);
				}

				return true; // event processed
			}
			virtual bool hover_panel(point2 const& position) override
			{
				if (m_hover) {
					element_type * found = find(position.y());

					if (found) m_hover(*found);
				}

				return true; // event processed
			}
			virtual bool scroll_panel(double horisontal, double vertical) override
			{
				int scroll = horisontal + vertical > 0 ? 1 : -1;
				scroll_list(-scroll);

				return true;
			}

		private:
			size_t count() const
			{
				size_t counter = 0;
				if (m_container) {
					m_container->enumerate([&](auto const& item) {
						if (m_filter(item)) {
							++counter;
						}
					});
				}
				return counter;
			}
			element_type * find(int selected) const
			{
				element_type * found = nullptr;

				if (m_container) {
					int index = 0 - m_scroll;
					m_container->enumerate([&](auto & item) {
						if (m_filter(item)) {
							if (selected == index) found = std::addressof(item);
							++index;
						}
					});
				}

				return found;
			}

			void scroll_list(int scroll)
			{
				m_scroll += scroll;

				int height = bounds().range().y();
				int total = static_cast<int>(count());

				m_scroll = std::min(m_scroll, total - height + 1);
				m_scroll = std::max(0, m_scroll);
			}

		private:
			container_type *	m_container;

			int					m_scroll;

			color				m_color;
			text_alignment		m_text_alignment;

			filter_fn			m_filter;
			format_fn			m_format;	// item to string
			event_fn			m_click;	// on click event callback
			event_fn			m_hover;	// on hover event callback
		};
	}
}