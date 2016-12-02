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
		class buff : public rl::enhancement_collection<Effect>, public entity
		{
		public:
			typedef Effect effect_type;
			typedef EventOperator event_type;

		public:
			bool expired() const noexcept
			{
				return !m_permanent && m_time == 0;
			}
			template <typename... Args>
			void apply(Args &&... args)
			{
				if (m_apply)
				{
					m_apply(std::forward<Args>(args)...);
				}
			}
			template <typename... Args>
			void expire(Args &&... args)
			{
				if (m_expire)
				{
					m_expire(std::forward<Args>(args)...);
				}
			}
			template <typename... Args>
			void tick(Args &&... args)
			{
				if (!m_permanent)
				{
					--m_time;
				}
				if (m_tick)
				{
					m_tick(std::forward<Args>(args)...);
				}
			}

		public:
			buff(unsigned int duration) noexcept : m_permanent(false), m_time(duration)
			{
			}
			buff() noexcept : m_permanent(true), m_time(0)
			{
			}

		private:
			bool m_permanent;
			int m_time;
			event_type m_tick;
			event_type m_apply;
			event_type m_expire;
		};
	}
}