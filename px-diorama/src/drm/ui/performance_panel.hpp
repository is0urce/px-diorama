// name: performance_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/common/timer.hpp>
#include <px/common/fps_counter.hpp>
#include <px/ui/panel.hpp>

#include <px/rglfw/glfw_time.hpp>

#include <memory>

namespace px {
	namespace ui {

		class performance_panel
			: public panel
		{
		public:
			typedef timer<glfw_time> timer_type;
			typedef fps_counter<timer_type> counter_type;

		public:
			virtual ~performance_panel()
			{
			}
			performance_panel()
				: m_fps(std::make_unique<counter_type>(&m_timer))
			{
			}
			virtual void draw_panel(display & window) const override
			{
				window.print({ 0, 0 }, 0xffffff, std::to_string(m_fps->fps()));
				m_fps->frame();
			}

		protected:

		private:
			timer_type m_timer;
			std::unique_ptr<counter_type> m_fps;
		};
	}
}