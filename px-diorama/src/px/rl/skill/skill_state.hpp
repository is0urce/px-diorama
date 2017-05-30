// name: skill_base.hpp
// type: c++ header
// desc: class
// auth: is0urce

// skill base attributes

#pragma once

#include <px/rl/entity.hpp>
#include <string>

namespace px
{
	namespace rl
	{
		class skill_state : public entity
		{
		public:
			typedef int time_type;
			typedef int range_type;

		public:

			// attrubute get

			std::string alias() const
			{
				return m_alias;
			}
			time_type cooldown_duration() const noexcept
			{
				return m_cooldown;
			}
			time_type duration() const noexcept
			{
				return m_duration;
			}
			bool hostile() const noexcept
			{
				return m_hostile;
			}
			int cost() const noexcept
			{
				return m_cost;
			}
			range_type range_min() const noexcept
			{
				return m_min_range;
			}
			range_type range_max() const noexcept
			{
				return m_max_range;
			}

			// attribute set

			void set_alias(std::string short_name)
			{
				m_alias = short_name;
			}
			void set_range(range_type min_range, range_type max_range) noexcept
			{
				m_min_range = min_range;
				m_max_range = max_range;
			}
			void set_cooldown(time_type cooldown_duration) noexcept
			{
				m_cooldown = cooldown_duration;
			}
			void set_cost(int cost) noexcept
			{
				m_cost = cost;
			}
			void set_duration(int turns) noexcept
			{
				m_duration = turns;
			}
			void set_hostile(bool is_hostile) noexcept
			{
				m_hostile = is_hostile;
			}
			void set_hostile() noexcept
			{
				m_hostile = true;
			}
			template <bool HostileFlag>
			void set_hostile() noexcept
			{
				m_hostile = HostileFlag;
			}

			// aux

			bool in_range(range_type distance) const noexcept
			{
				return distance >= m_min_range && (m_max_range < 0 || distance <= m_max_range);
			}
			bool is_cooldown() const noexcept
			{
				return m_timer != 0;
			}
			time_type cooldown_remaining() const noexcept
			{
				return m_timer;
			}
			void start_cooldown() noexcept
			{
				m_timer = m_cooldown;
			}
			void reset_cooldown() noexcept
			{
				m_timer = 0;
			}
			void cooldown_by(time_type by_time)
			{
				m_timer = (m_timer < by_time) ? 0 : (m_timer - by_time);
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(static_cast<entity &>(*this));
				archive(m_cooldown, m_timer, m_duration, m_min_range, m_max_range, m_cost, m_hostile);
			}

		public:
			skill_state()
				: m_cooldown(0)
				, m_timer(0)
				, m_duration(0)
				, m_min_range(-1)
				, m_max_range(-1)
				, m_cost(0)
				, m_hostile(false)
			{
			}

		private:
			time_type	m_cooldown;		// remaining time to ready state
			time_type	m_timer;		// cooldown duration
			time_type	m_duration;		// turns to use

			range_type	m_min_range;	// min use distance
			range_type	m_max_range;	// max use distance

			int			m_cost;			// resource required to use
			bool		m_hostile;		// is considered as hostile by npc
			std::string	m_alias;		// short name
		};
	}
}