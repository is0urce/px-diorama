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
			m_environment->popup(location, text, rgba, size);
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