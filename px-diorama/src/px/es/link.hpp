// name: link
// type: c++ header
// auth: is0urce
// desc: link to other components

#pragma once

#include <px/es/link_dispatcher.hpp>

namespace px {
	namespace es {

		template <typename T>
		class link
		{
		public:
			link()
				: m_link(nullptr)
			{
			}
			link(T * element)
				: m_link(element)
			{
			}

		public:
			T * get_link() const noexcept
			{
				return m_link;
			}
			void set_link(T * element) noexcept
			{
				m_link = element;
			}
			void break_link() noexcept
			{
				m_link = nullptr;
			}
			operator T * () const noexcept
			{
				return m_link;
			}

		private:
			T* m_link;
		};
	}
}