// name: link
// type: c++ header
// auth: is0urce
// desc: link to other components

#pragma once

namespace px
{
	template <typename T>
	class link
	{
	public:
		link()
			: m_link(nullptr)
		{
		}
		link(T* element)
			: m_link(element)
		{}

	public:
		T * get_link() noexcept
		{
			return m_link;
		}
		T const* get_link() const noexcept
		{
			return m_link;
		}
		void set_link(T* element) noexcept
		{
			m_link = element;
		}
		void break_link() noexcept
		{
			m_link = nullptr;
		}
		operator T const*() const noexcept
		{
			return m_link;
		}
		operator T*() noexcept
		{
			return m_link;
		}

	private:
		T* m_link;
	};

	template <typename C>
	class link_dispatcher
	{
	public:
		template <typename T>
		void assign(T* x) noexcept
		{
			link<T>* l = static_cast<C*>(this);
			l->set_link(x);
		}
		template <typename T>
		void retract() noexcept
		{
			link<T>* l = static_cast<C*>(this);
			l->break_link();
		}

		template <typename T>
		T const* linked() const noexcept
		{
			link<T> const* l = static_cast<C const*>(this);
			return l->get_link();
		}
		template <typename T>
		T* linked() noexcept
		{
			link<T>* l = static_cast<C*>(this);
			return l->get_link();
		}
	};
}