// name: script_environment.cpp

// script bindings for game object unit

#pragma once

#include "script_unit.hpp"
#include "drm/environment.hpp"

#include <px/common/assert.hpp>
#include <string>
#include <tuple>

#include "rl/tile_terrain.hpp"

namespace px {

	class script_environment
	{
	public:
		int distance(point2 const& a, point2 const& b)
		{
			return m_environment->distance(a, b);
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
		void emit_decal(script_unit & target, std::string const& tag)
		{
			m_environment->visual(tag, target.last_position(), target.position(), target.transform());
		}

		std::tuple<int, int, bool, bool> hit(script_unit const& attacker, script_unit const& versus)
		{
			body_component * user = attacker.body();
			body_component * vs = versus.body();

			if (!user || !vs) return {};

			return m_environment->hit(*user, *vs);	
		}

		script_unit spawn(std::string const& blueprint_tag, point2 const& location)
		{
			transform_component * transform = nullptr;
			body_component * body = nullptr;
			px_assert(m_environment);

			try {
				auto unit_ptr = m_environment->import_unit(blueprint_tag);
				transform = unit_ptr ? unit_ptr->transform() : nullptr;
				body = transform ? transform->linked<body_component>() : nullptr;
				m_environment->spawn(unit_ptr, location);
			}
			catch (...)
			{
			}
			return script_unit(body, transform);
		}
		void mass_export(point2 const& location)
		{
			px_assert(m_environment);
			m_environment->mass_export(location);
		}

		void pset(uint32_t tile_index, point2 const& location)
		{
			px_assert(m_environment);
			m_environment->terrain().pset(location, tile_index);
		}
		
		void damage(script_unit & mob, int damage)
		{
			auto * body = mob.body();

			px_assert(m_environment);
			px_assert(body);

			if (body && m_environment) {
				m_environment->damage(*body, damage);
			}
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