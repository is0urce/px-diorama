// name: target_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/transform_component.hpp"

namespace px {
	namespace ui {

		class target_panel
			: public panel
		{
		public:
			void lock_target(transform_component * target)
			{
				m_target = target;
			}
			void lock_position(point2 position)
			{
				m_position = position;
			}
			void clear_target()
			{
				m_target = nullptr;
			}

		public:
			virtual ~target_panel()
			{
			}
			target_panel()
				: m_target(nullptr)
				, m_position(0, 0)
			{
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				window.paint(color{ 1, 1, 1, 0.5 });
				std::string txt("");
				if (m_target)
				{
					txt += std::string("[") + std::to_string(m_target->x()) + std::string(", ") + std::to_string(m_target->y()) + std::string("]");
					txt += " ";
				}
				txt += std::string("[") + std::to_string(m_position.x()) + std::string(", ") + std::to_string(m_position.y()) + std::string("]");
				window.print({ 0, 0 }, 0x000000, txt);
			}
			virtual bool click_panel(point2 const& /*position*/, int /*button*/) const override
			{
				return true;
			}

		private:
			point2 m_position;
			transform_component * m_target;
		};
	}
}