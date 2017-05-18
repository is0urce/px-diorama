// name: script_environment.cpp

// script bindings for game object unit

#pragma once

#include "script_unit.hpp"
#include "drm/environment.hpp"

#include <string>

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
			m_environment->emit_vfx(location, tag);
		}
		void emit_projectile(point2 start, point2 end, std::string const& tag)
		{
			m_environment->emit_projectile(start, end, tag);
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