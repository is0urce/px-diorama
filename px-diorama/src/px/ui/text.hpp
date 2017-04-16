// name: text.hpp
// type: c++
// desc: class
// auth: isource

// text widget

#pragma once

#include "panel.hpp"
#include "text_alignment.hpp"

#include <functional>
#include <string>

namespace px {
	namespace ui {

		class text
			: public panel
		{
		public:
			typedef std::function<std::string()> text_fn;

		public:
			text(char const* c_str)
				: text([str = std::string(c_str)]() { return str; }, { 0, 0, 0, 1 }, text_alignment::left)
			{
			}
			text(std::string str)
				: text([str]() { return str; }, {0, 0, 0, 1}, text_alignment::left)
			{
			}
			text(text_fn txt)
				: text(txt, { 0, 0, 0, 1 }, text_alignment::left)
			{
			}
			text(char const* c_str, color front, text_alignment align)
				: text([str = std::string(c_str)]() { return str; }, front, align)
			{
			}
			text(std::string str, color front, text_alignment align)
				: text([str]() { return str; }, front, align)
			{
			}
			text(text_fn txt, color front, text_alignment align)
				: m_text(txt)
				, m_color(front)
				, m_alignment(align)
			{
			}

		public:
			auto current()
			{
				return m_text();
			}
			void set_color(color front)
			{
				m_color = front;
			}
			void set_alignment(text_alignment align)
			{
				m_alignment = align;
			}
			void set_text(std::string txt)
			{
				m_text = [str = txt]() { return str; };
			}
			void set_text(text_fn txt)
			{
				m_text = txt;
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				auto str = m_text();
				auto len = str.length();

				size_t offset = 0;
				switch (m_alignment) {
				case text_alignment::center:
					offset = len / 2;
					break;
				case text_alignment::right:
					offset = len;
					break;
				}

				window.print({ 0 + static_cast<int>(offset), 0 }, m_color, str);
			}

		private:
			color m_color;
			text_alignment m_alignment;
			text_fn m_text;
		};
	}
}