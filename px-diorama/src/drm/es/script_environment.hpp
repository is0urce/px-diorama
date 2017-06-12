// name: script_environment.cpp

// script bindings for game object unit

#pragma once

#include "script_unit.hpp"
#include "drm/environment.hpp"

#include <px/common/assert.hpp>
#include <string>
#include <tuple>

namespace px {

	class script_environment
	{
	public:
		int distance(point2 const& a, point2 const& b)
		{
			return a.king_distance(b);
		}
		void popup(point2 location, std::string text, int rgba, float size)
		{
			m_environment->popup(location, text, color(rgba), size);
		}
		void emit_vfx(point2 location, std::string const& tag)
		{
			m_environment->visual(tag, location, location, nullptr);
		}
		void emit_projectile(point2 start, point2 end, std::string const& tag)
		{
			m_environment->visual(tag, start, end, nullptr);
		}
		void emit_missile(point2 start, script_unit & target, std::string const& tag)
		{
			m_environment->visual(tag, start, target.position(), target.transform());
		}
		std::tuple<int, int, bool, bool> hit(script_unit const& attacker, script_unit const& versus)
		{
			body_component * user = attacker.body();
			body_component * vs = versus.body();

			if (!user || !vs) return {};

			return m_environment->hit(*user, *vs);	
		}
		script_unit spawn(std::string const& blueprint_name, point2 location)
		{
			px_assert(m_environment);

			auto unit = m_environment->import_unit(blueprint_name, location);
			m_environment->spawn(unit);
			auto transform = unit->transform();
			auto body = transform ? transform->linked<body_component>() : nullptr;
			return script_unit(body, transform);
		}

	public:
		script_environment() noexcept
			: m_environment(nullptr)
		{
		}
		script_environment(environment * environment) noexcept
			: m_environment(environment)
		{
		}

	private:
		environment * m_environment;
	};
}