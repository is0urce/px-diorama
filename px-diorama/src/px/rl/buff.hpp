// name: buff.hpp
// type: c++ header
// desc: struct
// auth: is0urce

#pragma once

#include <px/rl/enhancement_collection.hpp>
#include <px/rl/entity.hpp>

namespace px
{
	namespace rl
	{
		template <typename Effect, typename EventOperator>
		class buff
			: public entity
			, public rl::enhancement_collection<Effect>
		{
		public:
			typedef Effect effect_type;
			typedef EventOperator event_type;

		public:
			bool expired() const noexcept
			{
				return !m_permanent && m_time == 0;
			}
			unsigned int total_duration() const noexcept
			{
				return m_duration;
			}
			unsigned int timer() const noexcept
			{
				return m_time;
			}

			void on_apply(event_type apply_event)
			{
				m_apply = apply_event;
			}
			void on_expire(event_type expire_event)
			{
				m_expire = expire_event;
			}
			void on_tick(event_type tick_event)
			{
				m_tick = tick_event;
			}

			template <typename... Args>
			void tick(Args &&... args)
			{
				if (m_time == m_duration) do_apply(args...);
				if (m_time != 0) do_tick(args...);
				if (m_time != 0 && !m_permanent) --m_time;
				if (m_time == 0) do_expire(args...);
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(static_cast<entity &>(*this));
				archive(static_cast<rl::enhancement_collection<Effect> &>(*this));

				archive(m_permanent);
				archive(m_duration);
				archive(m_time);
			}

		public:
			buff(unsigned int duration) noexcept
				: m_permanent(false)
				, m_time(duration)
				, m_duration(duration)
			{
			}

			buff() noexcept
				: m_permanent(true)
				, m_time(1)
				, m_duration(2)
			{
				// different no-zero timers to avoid unlimited on_apply / on_expire
			}

		private:
			template <typename... Args>
			void do_apply(Args &&... args)
			{
				if (m_apply) {
					m_apply(std::forward<Args>(args)...);
				}
			}

			template <typename... Args>
			void do_expire(Args &&... args)
			{
				if (m_expire) {
					m_expire(std::forward<Args>(args)...);
				}
			}

			template <typename... Args>
			void do_tick(Args &&... args)
			{
				if (m_tick) {
					m_tick(std::forward<Args>(args)...);
				}
			}

		private:
			bool		m_permanent;
			int			m_time;
			int			m_duration;
			event_type	m_tick;
			event_type	m_apply;
			event_type	m_expire;
		};
	}
}