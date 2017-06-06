// name: status_panel.hpp
// type: c++
// desc: class
// auth: isource

#pragma once

#include <px/ui/panel.hpp>

#include <px/ui/bar.hpp>

#include "drm/es/transform_component.hpp"
#include "drm/es/body_component.hpp"

namespace px {
	namespace ui {

		class target_panel
			: public panel
		{
		public:
			void lock(transform_component * target, point2 position) noexcept
			{
				lock_position(position);
				lock_target(target);
			}
			void lock_target(transform_component * target) noexcept
			{
				m_target = target;
			}
			void lock_position(point2 position) noexcept
			{
				m_position = position;
			}
			void clear_target() noexcept
			{
				m_target = nullptr;
			}
			point2 position() const noexcept
			{
				return m_position;
			}
			transform_component * target() const noexcept
			{
				return m_target;
			}

		public:
			virtual ~target_panel()
			{
			}
			target_panel()
				: m_target(nullptr)
				, m_position(0, 0)
			{
				//auto hp = make<bar>("hp", { { 0.0, 0.0 },{ 0, 0 },{ 0, 1 },{ 0.5, 0.0 } }, 0xff0000, 0x000000, [this]() {
				//	double current = 0;
				//	double maximum = 0;
				//	auto * body = m_target ? m_target->linked<body_component>() : nullptr;
				//	if (body && body->health()) {
				//			current = static_cast<double>(body->health()->current());
				//			maximum = static_cast<double>(body->health()->maximum());
				//	}
				//	return std::tuple<double, double>(current, maximum);
				//});
				//auto hplabel = hp->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } }, "HP");
				//hplabel->set_color(0xffffff);
				//auto hpvalue = hp->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } }, [this]() {
				//	std::string result;
				//	auto * body = m_target ? m_target->linked<body_component>() : nullptr;
				//	if (body && body->health()) {
				//		result = std::to_string(body->health()->current());
				//	}
				//	return result;
				//});
				//hpvalue->set_color(0xffffff);
				//hpvalue->set_alignment(text_alignment::right);

				//auto mp = make<bar>({ { 0.0, 0.0 },{ 0, 1 },{ 0, 1 },{ 0.5, 0.0 } }, 0x0000ff, 0x000000, [this]() {
				//	double current = 0;
				//	double maximum = 0;
				//	auto * body = m_target ? m_target->linked<body_component>() : nullptr;
				//	if (body && body->energy()) {
				//		current = static_cast<double>(body->energy()->current());
				//		maximum = static_cast<double>(body->energy()->maximum());
				//	}
				//	return std::tuple<double, double>(current, maximum);
				//});
				//auto mplabel = mp->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } }, "EP");
				//mplabel->set_color(0xffffff);
				//auto mpvalue = hp->make<text>({ { 0.0, 0.0 },{ 0, 0 },{ 0, 0 },{ 1.0, 1.0 } }, [this]() {
				//	std::string result;
				//	auto * body = m_target ? m_target->linked<body_component>() : nullptr;
				//	if (body && body->energy()) {
				//		result = std::to_string(body->energy()->current());
				//	}
				//	return result;
				//});
				//mpvalue->set_color(0xffffff);
				//mpvalue->set_alignment(text_alignment::right);
			}

		protected:
			virtual void draw_panel(display & window) const override
			{
				auto * body = m_target ? m_target->linked<body_component>() : nullptr;
				auto * container = body ? body->linked<container_component>() : nullptr;

				window.paint(color{ 1, 1, 1, 0.5 });
				int line = 0;
				if (body) {
					window.print({ 0, ++line }, 0x000000, body->name());
				}

				if (container) {
					window.print({ 0, ++line }, 0x000000, "has container");
				}
			}
			virtual bool click_panel(point2 const& /*position*/, int /*button*/) override
			{
				return true;
			}

		private:
			point2 m_position;
			transform_component * m_target;
		};
	}
}