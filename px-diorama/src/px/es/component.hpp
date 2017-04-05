// name: component.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

namespace px {
	namespace es {

		class component
		{
		public:
			bool active() const noexcept
			{
				return m_active;
			}
			void activate()
			{
				if (!m_active)
				{
					activate_component();
					m_active = true;
				}
			}
			void deactivate()
			{
				if (m_active)
				{
					deactivate_component();
					m_active = false;
				}
			}

		public:
			component()
				: m_active(false)
			{
			}
			virtual ~component()
			{
				deactivate();
			}

		protected:
			virtual void activate_component() {}
			virtual void deactivate_component() {}

		private:
			bool m_active;
		};
	}
}