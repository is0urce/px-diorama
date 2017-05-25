// name: script_environment.cpp

// script bindings for game object unit

#pragma once

#include "script_unit.hpp"
#include "drm/environment.hpp"

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
			m_environment->visual(tag, location);
		}
		void emit_projectile(point2 start, point2 end, std::string const& tag)
		{
			m_environment->projectile(tag, start, end);
		}
		void emit_missile(point2 start, script_unit & target, std::string const& tag)
		{
			m_environment->projectile(tag, start, target.position(), target.transform());
		}
		std::tuple<int, int, bool, bool> hit(script_unit const&, script_unit const&)
		{
			int magnitude = 5;
			int variant = 1;
			bool is_hit = true;
			bool is_critical = true;
			return std::tuple<int, int, bool, bool>{ magnitude, variant, is_hit, is_critical };
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