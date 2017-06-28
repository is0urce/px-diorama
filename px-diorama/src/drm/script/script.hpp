// name: character_system.hpp

#pragma once

#include "script_unit.hpp"
#include "script_environment.hpp"

#include <px/common/assert.hpp>

#pragma warning(push)
#pragma warning(disable:4127) // warning C4127: conditional expression is constant
#pragma warning(disable:4702) // warning C4702: unreachable code
#include <sol/sol.hpp>
#pragma warning(pop)

namespace px {
	namespace es {

		class script final
		{
		public:
			void provide_environment(environment * bind)
			{
				m_lua["game"] = script_environment(bind);
			}

			template <typename Book>
			void load_skill(std::string const& path, Book & book)
			{
				typedef typename Book::state_type state_type;

				try
				{
					auto & sandbox = m_scripts[path] = sol::environment(m_lua, sol::create, m_lua.globals());
					m_lua.script_file(path, sandbox);

					// state props

					state_type props;

					std::string tag = sandbox["tag"].get_or<std::string>(path);
					std::string name = sandbox["name"].get_or(tag);
					std::string alias = sandbox["alias"].get_or(name);
					std::string description = sandbox["description"].get_or<std::string>("");
					bool targeted = sandbox["targeted"].get_or(false);
					bool hostile = sandbox["hostile"].get_or(false);
					int duration = sandbox["duration"].get_or(0);
					int cooldown = sandbox["cooldown"].get_or(0);
					int cost = sandbox["cost"].get_or(0);
					int min_range = sandbox["min_range"].get_or(-1);
					int max_range = sandbox["max_range"].get_or(-1);

					props.set_tag(tag);
					props.set_name(name);
					props.set_description(description);
					props.set_alias(alias);
					props.set_hostile(hostile);
					props.set_duration(duration);
					props.set_cooldown(cooldown);
					props.set_cost(cost);
					props.set_range(min_range, max_range);

					// functional
					auto target_action = [&sandbox](body_component * user, body_component * target) -> void {
						try
						{
							sandbox["action"](script_unit(user), script_unit(target));
						}
						catch (sol::error & lua_error)
						{
							px_assert_nothrow(lua_error);
						}
					};
					auto target_condition = [&sandbox](body_component * user, body_component * target) -> bool {
						try
						{
							sol::function_result script_result = sandbox["condition"](script_unit(user), script_unit(target));
							return script_result.get<bool>();
						}
						catch (sol::error & lua_error)
						{
							px_assert_nothrow(lua_error);
							return false;
						}
					};
					auto area_action = [&sandbox](body_component * user, point2 const& area) -> void {
						try
						{
							sandbox["action"](script_unit(user), area);
						}
						catch (sol::error & lua_error)
						{
							px_assert_nothrow(lua_error);
						}
					};
					auto area_condition = [&sandbox](body_component * user, point2 const& area) -> bool {
						try
						{
							sol::function_result script_result = sandbox["condition"](script_unit(user), area);
							return script_result.get<bool>();
						}
						catch (sol::error & lua_error)
						{
							px_assert_nothrow(lua_error);
							return false;
						}
					};

					// create record

					book.emplace(tag, props, targeted, target_action, target_condition, area_action, area_condition);
				}
				catch (sol::error & script_error)
				{
					px_assert_nothrow(script_error);
				}
				catch (...)
				{
					throw std::runtime_error("px::script::load_skill(path), path=" + path);
				}
			}

		public:
			script()
			{
				m_lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math);
				
				declare_types();
				include_libraries();

				provide_environment(nullptr);
			}

		private:
			void declare_types()
			{
				m_lua.new_usertype<script_unit>("unit"
					, "place", &script_unit::place
					, "position", &script_unit::position
					, "dead", &script_unit::dead
					, "damage", &script_unit::damage
					);
				m_lua.new_usertype<point2>("point"
					, "x", &point2::x
					, "y", &point2::y
					);
				m_lua.new_usertype<script_environment>("environment"
					, "hit", &script_environment::hit
					, "distance", &script_environment::distance
					, "popup", &script_environment::popup
					, "vfx", &script_environment::emit_vfx
					, "vfx_projectile", &script_environment::emit_projectile
					, "vfx_missile", &script_environment::emit_missile
					, "mass_export", &script_environment::mass_export
					, "spawn", &script_environment::spawn
					, "pset", &script_environment::pset
					);
			}
			void include_libraries()
			{
				try
				{
					m_lua.script_file("data/scripts/i_damage.lua");
				}
				catch (...)
				{
					throw std::runtime_error("px::script::load_libraries() error");
				}
			}

		private:
			sol::state								m_lua;
			std::map<std::string, sol::environment> m_scripts;
		};
	}
}