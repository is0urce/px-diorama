// name: display.hpp
// type: c++
// desc: class declaration
// auth: isource

#pragma once

#include "canvas.hpp"

#include <px/common/rectangle.hpp>

namespace px {
	namespace ui {

		class display final
		{
		public:
			void set_frame(rectangle frame) noexcept
			{
				m_bounds = frame;
			}
			void assign(canvas * cnv) noexcept
			{
				m_canvas = cnv;
			}
			point2 start() const noexcept
			{
				return m_bounds.start();
			}
			rectangle const& bounds() const noexcept
			{
				return m_bounds;
			}

			// drawing

			void paint(color const& back)
			{
				m_canvas->paint(m_bounds, back);
			}
			void paint(rectangle const& bounds, color const& back)
			{
				rectangle absolute(bounds.start() + m_bounds.start(), bounds.range());
				m_canvas->paint(absolute.intersection(m_bounds), back);
			}
			void pset(point2 position, color const& back)
			{
				position += m_bounds.start();
				if (m_bounds.contains(position))
				{
					m_canvas->pset(position, back);
				}
			}
			void print(point2 position, color const& front, canvas::code_type code)
			{
				position += m_bounds.start();
				if (m_bounds.contains(position)) {
					m_canvas->print(position, front, code);
				}
			}
			void print(point2 position, color const& front, std::string const& text)
			{
				int space = m_bounds.width() - position.x();
				if (space > 0) {
					position += m_bounds.start();
					if (m_bounds.contains(position)) {
						m_canvas->print_n(position, front, text, space);
					}
				}
			}
			void print_n(point2 position, const color &front, std::string const& text, size_t max)
			{
				int space = m_bounds.width() - position.x();
				if (space > 0) {
					max = std::min(static_cast<int>(max), space);
					position += m_bounds.start();
					if (m_bounds.contains(position)) {
						m_canvas->print_n(position, front, text, max);
					}
				}
			}

			canvas const* get_canvas() const noexcept
			{
				return m_canvas;
			}

		public:
			display(canvas * cnv, rectangle frame) noexcept
				: m_canvas(cnv)
				, m_bounds(frame)
			{

			}
			display() noexcept
				: display(nullptr, {})
			{
			}

		private:
			canvas * m_canvas;
			rectangle m_bounds;
		};
	}
}