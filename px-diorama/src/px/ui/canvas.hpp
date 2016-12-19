// name: canvas.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/common/color.hpp>
#include <px/common/matrix.hpp>
#include <px/common/rectangle.hpp>
#include <px/common/string.hpp>

namespace px
{
	namespace ui
	{
		class canvas  final
		{
		public:
			typedef unsigned int code_type;

		public:
			void resize(size_t width, size_t height)
			{
				m_codes.resize(width, height);
				m_front.resize(width, height);
				m_back.resize(width, height);
			}

			// writing
			void cls()
			{
				cls(' ', { 0, 0, 0, 0 });
			}
			void cls(code_type code, color const& back)
			{
				m_codes.fill(code);
				m_back.fill(back);
			}
			void paint(rectangle const& bounds, color const& back)
			{
				bounds.intersection(m_back.range()).enumerate([&](point2 const& location)
				{
					m_codes[location] = ' ';
					m_back[location] = back;
				});
			}
			void pset(point2 const& position, color const& back)
			{
				if (m_back.contains(position))
				{
					m_back[position] = back;
				}
			}
			void print(point2 const& position, color const& front, code_type code)
			{
				if (m_codes.contains(position))
				{
					m_codes[position] = code;
					m_front[position] = front;
				}
			}
			void print(point2 position, color const& front, std::string const& text)
			{
				string::enumerate_utf8(text, [&](auto code)
				{
					print(position, front, code);
					position.move_axis<0>(1);
				});
			}
			size_t print_n(point2 position, const color &front, std::string const& text, size_t max)
			{
				string::enumerate_utf8(text, [&](auto code)
				{
					if (max != 0)
					{
						print(position, front, code);
						position.move_axis<0>(1);
						--max;
					}
				});
				return max;
			}

		public:
			canvas()
			{
			}
			canvas(size_t width, size_t height)
				: m_codes(width, height)
				, m_front(width, height)
				, m_back(width, height)
			{
			}

		private:
			matrix2<code_type> m_codes;
			matrix2<color> m_front;
			matrix2<color> m_back;
		};
	}
}