// name: skill_base.hpp
// type: c++ header
// desc: class
// auth: is0urce

// skill base attributes

#pragma once

#include <tuple>

#include <px/rl/entity.hpp>

namespace px
{
	namespace rl
	{
		class skill_base : public entity
		{
		public:
			typedef unsigned int time_type;
			typedef unsigned int range_component;
			typedef std::pair<range_component, range_component> range;
		private:
			time_type m_cooldown;
			time_type m_timer;
			bool m_hostile;
			std::string m_alias; // short name;

		public:
			skill_base()
				: m_cooldown(0), m_timer(0), m_hostile(false)
			{}
			virtual ~skill_base() {}

		public:
			void set_cooldown_time(time_type time)
			{
				m_cooldown = time;
			}
			void set_cooldown_remaining(time_type time)
			{
				m_timer = time;
			}
			time_type cooldown_time() const
			{
				return m_cooldown;
			}
			time_type cooldown_remaining() const
			{
				return m_timer;
			}
			bool on_cooldown() const
			{
				return m_timer > 0;
			}
			void cooldown(time_type time)
			{
				m_timer = (m_timer < time) ? 0 : m_timer - time;
			}
			void reset_cooldown()
			{
				m_timer = 0;
			}
			void start_cooldown()
			{
				m_timer = m_cooldown;
			}

			bool hostile() const
			{
				return m_hostile;
			}
			void set_hostile(bool flag)
			{
				m_hostile = flag;
			}

			auto alias() const
			{
				return m_alias;
			}
			void set_alias(std::string short_name)
			{
				m_alias = short_name;
			}
		};
	}
}