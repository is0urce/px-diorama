// name: resource.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/bar.hpp>

namespace px
{
	namespace rl
	{
		template<typename T>
		class resource
			: public bar<T>
		{
		public:
			typedef bar<T> bar;

		public:
			void set(T amount)
			{
				create();
				bar::set(amount);
			}
			void set(T current, T max)
			{
				create();
				bar::set(current, max);
			}
			void create() noexcept
			{
				m_enabled = true;
			}
			void remove() noexcept
			{
				m_enabled = false;
			}

			operator bool() const noexcept
			{
				return m_enabled;
			}

		public:
			resource() noexcept
				: m_enabled(false)
			{
			}

		private:
			bool m_enabled;
		};
	}
}