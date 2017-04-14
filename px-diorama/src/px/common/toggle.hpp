// name: toggle.hpp
// type: c++ header
// desc: template

// okay, stop reinventing this over and over
// almost as bad as iheriting from bool

#pragma once

namespace px {

	template <bool Initial>
	class toggle
	{
	public:
		bool active() const noexcept
		{
			return m_switch;
		}

		void activate() noexcept
		{
			m_switch = true;
		}
		void deactivate() noexcept
		{
			m_switch = false;
		}
		void reverse_toggle() noexcept
		{
			m_switch = !m_switch;
		}
		void reset() noexcept
		{
			m_switch = Initial;
		}
		void set_toggle(bool flag) noexcept
		{
			m_switch = flag;
		}

	public:
		constexpr toggle() noexcept
			: m_switch(Initial)
		{
		}

	private:
		bool m_switch;
	};
}