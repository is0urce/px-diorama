// name: intrusive_ptr.hpp

#pragma once

#include <memory>

namespace px {

	template <typename T, typename Deleter>
	class intrusive_ptr;

	template <typename T, typename Deleter = std::default_delete<T>>
	class intrusive_ptr
	{
	public:
		void swap(intrusive_ptr & rhs)
		{
			std::swap(m_pointer, rhs.m_pointer);
			std::swap(m_deleter, rhs.m_deleter);
		}

		T * get() const noexcept
		{
			return m_pointer;
		}
		void reset() noexcept
		{
			intrusive_ptr().swap(*this);
		}
		void reset(T * ptr)
		{
			intrusive_ptr(ptr).swap(*this);
		}
		void reset(T * ptr, Deleter d)
		{
			intrusive_ptr(ptr, d).swap(*this);
		}

		T & operator*() const noexcept
		{
			return *m_pointer;
		}
		T * operator->() const noexcept
		{
			return m_pointer;
		}
		operator bool() const noexcept
		{
			return m_pointer != nullptr;
		}

	public:
		intrusive_ptr()
			: m_pointer(nullptr)
		{
		}
		intrusive_ptr(T * ptr, Deleter d = Deleter{})
			: m_pointer(ptr)
			, m_deleter(d)
		{
			if (m_pointer) m_pointer->increment_intrusive();
		}
		intrusive_ptr(intrusive_ptr const& rhs)
			: m_pointer(rhs.get())
			, m_deleter(rhs.m_deleter)
		{
			if (m_pointer) m_pointer->increment_intrusive();
		}
		intrusive_ptr(intrusive_ptr && rhs) noexcept
			: m_pointer(rhs.m_pointer)
		{
			rhs.m_pointer = nullptr;
		}
		intrusive_ptr & operator=(intrusive_ptr && rhs) noexcept
		{
			intrusive_ptr(std::move(rhs)).swap(*this);
			return *this;
		}
		intrusive_ptr & operator=(intrusive_ptr const& rhs)
		{
			intrusive_ptr(rhs).swap(*this);
			return *this;
		}
		intrusive_ptr & operator=(T * rhs)
		{
			intrusive_ptr(rhs).swap(*this);
			return *this;
		}

		~intrusive_ptr()
		{
			if (m_pointer && m_pointer->decrement_intrusive() == 0)
			{
				m_deleter(m_pointer);
			}
		}

	private:
		T * m_pointer;
		Deleter m_deleter;
	};

	template<class T, class U> inline bool operator==(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b) {	return a.get() == b.get(); }
	template<class T, class U> inline bool operator!=(intrusive_ptr<T> const& a, intrusive_ptr<U> const& b) { return a.get() != b.get(); }
	template<class T, class U> inline bool operator==(intrusive_ptr<T> const& a, U * b) { return a.get() == b; }
	template<class T, class U> inline bool operator!=(intrusive_ptr<T> const& a, U * b) { return a.get() != b; }
	template<class T, class U> inline bool operator==(T * a, intrusive_ptr<U> const& b) { return a == b.get(); }
	template<class T, class U> inline bool operator!=(T * a, intrusive_ptr<U> const& b) { return a != b.get(); }

	template<class T> inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
	{
		return std::less<T*>()(a.get(), b.get());
	}
	template<class T> void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
	{
		lhs.swap(rhs);
	}

	class intrusive
	{
	public:
		void increment_intrusive()
		{
			++m_counter;
		}
		unsigned int decrement_intrusive()
		{
			--m_counter;
			return m_counter;
		}
		unsigned int intrusive_counter()
		{
			return m_counter;
		}
	public:
		intrusive()
			: m_counter(0)
		{
		}
	private:
		unsigned int m_counter;
	};
}