// name: fps_counter.h
// type: c++ header
// desc: class declaration

// class dedicated to count fps

#pragma once

namespace px
{
	template <typename timer>
	class fps_counter
	{
	public:
		const float interval = 0.2f;

	public:
		fps_counter(timer * timer)
			: m_timer(timer)
			, m_frames(0)
			, m_fps(0)
		{
			m_last = m_timer->measure();
		}

	public:
		auto measure() const noexcept
		{
			return m_fps;
		}
		void frame()
		{
			++m_frames;
			auto current = m_timer->measure();
			auto delta = current - m_last;

			if (delta > interval)
			{
				// calculate fps
				m_fps = m_frames / delta;

				// restart counters
				m_frames = 0;
				m_last = current;
			}
		}
		auto operator()() const noexcept
		{
			return m_fps;
		}

	private:
		timer * m_timer;
		unsigned int m_frames;
		decltype(m_timer->measure()) m_fps;
		decltype(m_timer->measure()) m_last;
	};
}