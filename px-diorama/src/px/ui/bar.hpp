// name: board.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include "panel.hpp"

#include <functional>

namespace px {
	namespace ui {

		class bar
			: public panel
		{
		public:
			typedef std::function<std::tuple<double, double>()> query_fn;

		public:
			void set_query(query_fn query)
			{
				m_query = query;
			}

		public:
			virtual ~bar()
			{
			}
			bar()
				: bar(0, 0, nullptr)
			{
			}
			bar(color full, color empty)
				: bar(full, empty, nullptr)
			{
			}
			bar(color full, color empty, query_fn query)
				: m_full(full)
				, m_empty(empty)
				, m_query(query)
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				window.paint(m_empty);

				if (m_query) {
					double current;
					double maximum;
					std::tie(current, maximum) = m_query();
					double percent = current / maximum;

					int width = bounds().width();
					int height = bounds().height();
					int filled = static_cast<int>(width * percent);

					if (current > 0 && filled == 0) filled = 1; // min padding
					if (current < maximum && filled == width) filled -= 1; // max padding

					window.paint(rectangle{ {0, 0}, { filled, height } }, m_full);
				}
			}

		private:
			color		m_full;
			color		m_empty;
			query_fn	m_query;
		};
	}
}