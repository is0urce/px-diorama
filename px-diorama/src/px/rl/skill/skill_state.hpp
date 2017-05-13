// name: skill_base.hpp
// type: c++ header
// desc: class
// auth: is0urce

// skill base attributes

#pragma once

#include <px/rl/entity.hpp>

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
				archive(m_cooldown, m_timer, m_hostile, m_instant, m_min_range, m_max_range);
			}

			// attrubute get

			time_type cooldown_duration() const noexcept
			{
				return m_cooldown;
			}
			bool instant() const noexcept
			{
				return m_instant;
			}
			bool hostile() const noexcept
			{
				return m_hostile;
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

			void set_range(range_type min_range, range_type max_range) noexcept
			{
				m_min_range = min_range;
				m_max_range = max_range;
			}
			void set_cooldown(time_type cooldown_duration) noexcept
			{
				m_cooldown = cooldown_duration;
			}
			void set_instant(bool is_instant) noexcept
			{
				m_instant = is_instant;
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

		public:
			skill_state()
				: m_cooldown(0)
				, m_timer(0)
				, m_hostile(false)
				, m_instant(false)
				, m_min_range(-1)
				, m_max_range(-1)
			{
			}

		private:
			time_type	m_cooldown;		// remaining time to ready state
			time_type	m_timer;		// cooldown duration
			bool		m_hostile;		// is considered as hostile by npc
			bool		m_instant;		// is turn not passed after use
			range_type	m_min_range;	// min use distance
			range_type	m_max_range;	// max use distance
		};
	}
}