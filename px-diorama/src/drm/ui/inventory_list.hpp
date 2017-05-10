// name: inventory_list.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/container_component.hpp"

#include <functional>
#include <stdexcept>
#include <typeinfo>

namespace px {
	namespace ui {

		class inventory_list
			: public panel
		{
		public:
			typedef std::shared_ptr<rl::item> element_type;
			typedef std::function<void(element_type &)> click_fn;
			typedef std::function<std::string(element_type const&)> format_fn;
			typedef std::function<bool(element_type const&)> filter_fn;

		public:
			void assign_container(container_component * container) noexcept
			{
				m_container = container;
			}
			void detach_container() noexcept
			{
				m_container = nullptr;
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
			container_component * list()
			{
				return m_container;
			}

		public:
			virtual ~inventory_list()
			{
			}
			inventory_list()
				: m_container(nullptr)
				, m_color(0x000000)
				, m_format([](auto const&) { return typeid(element_type).name(); })
				, m_filter([](auto const&) { return true; })
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				if (m_container && m_format)
				{
					int index = 0;
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

					int index = 0;
					m_container->enumerate([&](auto & item) {
						if (m_filter(item)) {
							if (selected == index) found = &item;
							++index;
						}
					});

					if (found) m_click(*found);
				}

				return true; // event processed
			}

		private:
			container_component * m_container;

			color		m_color;

			filter_fn	m_filter;
			format_fn	m_format;	// item to string
			click_fn	m_click;	// on click event callback
		};
	}
}