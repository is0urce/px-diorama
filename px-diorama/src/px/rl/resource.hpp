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
		template <typename T>
		class resource
		{
		public:
			typedef bar<T> bar_type;

		public:
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

			bar_type * operator->() noexcept {
				m_enabled = true;
				return &m_bar;
			}
			bar_type const* operator->() const noexcept {
				return &m_bar;
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_enabled);
				if (m_enabled) {
					m_mar.serialize(archive);
				}
			}

		public:
			resource() noexcept
				: m_enabled(false)
			{
			}

		private:
			bool m_enabled;
			bar_type m_bar;
		};
	}
}