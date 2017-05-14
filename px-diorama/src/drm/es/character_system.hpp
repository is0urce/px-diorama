// name: character_system.hpp

#pragma once

#include "character_component.hpp"

#include "script_unit.hpp"
#include "script_environment.hpp"

#include <px/es/basic_system.hpp>

#include <px/rl/skill/skill_book.hpp>

#pragma warning(push)
#pragma warning(disable:4127) // warning C4127: conditional expression is constant
#pragma warning(disable:4702) // warning C4702: unreachable code
#include <sol/sol.hpp>
#pragma warning(pop)

namespace px {
	namespace es {

		class character_system final
			: public basic_system<character_component, 100000>
		{
		public:
			typedef character_component::book_type book_type;
			typedef book_type::state_type state_type;

		public:
			auto make_shared()
			{
				auto result = basic_system::make_shared();
				setup_component(*result);
				return result;
			}
			auto make_unique()
			{
				auto result = basic_system::make_unique();
				setup_component(*result);
				return result;
			}
			auto make_std()
			{
				auto result = basic_system::make_std();
				setup_component(*result);
				return result;
			}

			void provide_environment(environment * bind)
			{
				m_lua["game"] = script_environment(bind);
			}
			void load_skill(char const* path)
			{
				m_lua.script_file(path);
				
				sol::optional<std::string> maybe_tag = m_lua["tag"];

				if (!maybe_tag) throw std::runtime_error("px::rl::character_system::load_skill(path) - no tag, path = " + std::string(path));

				// state props

				state_type props;

				std::string tag = maybe_tag.value();
				std::string name = m_lua["name"].get_or(tag);
				std::string alias = m_lua["alias"].get_or(name);
				std::string description = m_lua["description"].get_or(std::string(""));
				bool targeted = m_lua["targeted"].get_or(false);
				bool hostile = m_lua["hostile"].get_or(false);
				bool instant = m_lua["instant"].get_or(false);
				int cooldown = m_lua["cooldown"].get_or(0);
				int cost = m_lua["cost"].get_or(0);
				int min_range = m_lua["min_range"].get_or(-1);
				int max_range = m_lua["max_range"].get_or(-1);

				props.set_tag(tag);
				props.set_name(name);
				props.set_description(description);
				props.set_hostile(hostile);
				props.set_instant(instant);
				props.set_cooldown(cooldown);
				props.set_cost(cost);
				props.set_range(min_range, max_range);

				// functional

				std::function<void(body_component *, body_component *)> target_action;
				std::function<bool(body_component *, body_component *)> target_condition;
				std::function<void(body_component *, point2 const&)> area_action;
				std::function<bool(body_component *, point2 const&)> area_condition;

				if (targeted) {
					std::function<void(script_unit, script_unit)> script_action = m_lua["action"];
					std::function<bool(script_unit, script_unit)> script_condition = m_lua["condition"];

					target_action = [script_action](body_component * user, body_component * target) {
						try
						{
							script_action(user, target);
						}
						catch (...)
						{
						}
					};
					target_condition = [script_condition](body_component * user, body_component * target) {
						try
						{
							return script_condition(user, target);
						}
						catch (...)
						{
							return false;
						}
					};
				}
				else {
					std::function<void(script_unit, point2 const&)> script_action = m_lua["action"];
					std::function<bool(script_unit, point2 const&)> script_condition = m_lua["condition"];

					area_action = [script_action](body_component * user, point2 const& area) { 
						try
						{
							script_action(user, area);
						}
						catch (...)
						{
						}
					};
					area_condition = [script_condition](body_component * user, point2 const& area) {
						try
						{
							return script_condition(user, area);
						}
						catch (...)
						{
							return false;
						}
					};
				}

				// create record

				m_book.emplace(tag, props, targeted, target_action, target_condition, area_action, area_condition);
			}

		public:
			character_system()
			{
				m_lua.open_libraries(sol::lib::base, sol::lib::package);

				m_lua.script_file("data/scripts/damage_types.lua");

				m_lua.new_usertype<script_unit>("unit"
					, "place", &script_unit::place
					, "position", &script_unit::position);
				m_lua.new_usertype<point2>("point");
				m_lua.new_usertype<script_environment>("environment"
					, "distance", &script_environment::distance);

				provide_environment(nullptr);

				load_skill("data/scripts/teleport.lua");
			}

		private:
			void setup_component(character_component & element)
			{
				element.skills().provide_book(&m_book);
			}

		private:
			book_type	m_book;
			sol::state	m_lua;
		};
	}
}