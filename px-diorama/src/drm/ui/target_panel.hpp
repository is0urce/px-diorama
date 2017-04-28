// name: target_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include "drm/es/transform_component.hpp"
#include "drm/es/body_component.hpp"

namespace px {
	namespace ui {

		class target_panel
			: public panel
		{
		public:
			void lock(point2 position, transform_component const* target)
			{
				lock_position(position);
				lock_target(target);
			}
			void lock_target(transform_component const* target)
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
				txt += std::string("[") + std::to_string(m_position.x()) + std::string(", ") + std::to_string(m_position.y()) + std::string("]");

				if (m_target) {
					txt += " OBJ ";
					if (auto body = m_target->linked<body_component>())
					{
						txt += body->name();
						if (auto inventory = body->linked<container_component>())
						{
							txt += " INV";
						}
					}
				}
				window.print({ 0, 0 }, 0x000000, txt);
			}
			virtual bool click_panel(point2 const& /*position*/, int /*button*/) override
			{
				return true;
			}

		private:
			point2 m_position;
			transform_component const* m_target;
		};
	}
}