// name: list.hpp
// type: c++
// desc: template class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

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
			typedef typename Container::element_type element_type;
			typedef std::function<void(element_type &)> click_fn;
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
			void on_click(click_fn click_action)
			{
				m_click = click_action;
			}
			container_type * assigned_container()
			{
				return m_container;
			}

		public:
			virtual ~list()
			{
			}
			list()
				: m_container(nullptr)
				, m_color(0x000000)
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
					m_container->enumerate([&](auto const& item) {
						if (m_filter(item)) {
							window.print({ 0, 0 + index }, m_color, m_format(item));
							++index;
						}
					});
				}
			}
			virtual bool click_panel(point2 const& position, int mouse_button) override
			{
				if (m_container && mouse_button == 0 && m_click)
				{
					element_type * found = nullptr;
					int selected = position.y();

					int index = 0 - m_scroll;
					m_container->enumerate([&](auto & item) {
						if (m_filter(item)) {
							if (selected == index) found = std::addressof(item);
							++index;
						}
					});

					if (found) m_click(*found);
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

			filter_fn			m_filter;
			format_fn			m_format;	// item to string
			click_fn			m_click;	// on click event callback
		};
	}
}