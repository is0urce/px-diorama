// name: skill_base.hpp
// type: c++ header
// desc: class
// auth: is0urce

// skill base attributes

#pragma once

#include <px/rl/entity.hpp>

#include <tuple>

namespace px
{
	namespace rl
	{
		class skill_state : public entity
		{
		public:
			typedef unsigned int time_type;
			typedef int range_component;
			typedef std::pair<range_component, range_component> range;

		public:
			void set_cooldown(time_type time)
			{
				m_cooldown = time;
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
				return m_timer != 0;
			}
			void cooldown_by(time_type by_time)
			{
				m_timer = (m_timer < by_time) ? 0 : (m_timer - by_time);
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
			void set_hostile()
			{
				m_hostile = true;
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_cooldown);
			}

		public:
			skill_state()
				: m_cooldown(0)
				, m_timer(0)
				, m_hostile(false)
			{
			}

		private:
			time_type	m_cooldown; // remaining time to ready state
			time_type	m_timer;	// cooldown duration
			bool		m_hostile;	// is considered as hostile by npc
		};
	}
}