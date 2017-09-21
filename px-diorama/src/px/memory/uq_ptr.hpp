// name: uq_ptr
// type: c++ header

#pragma once

// unique_ptr but with erased deleter

#include <px/memory/poly_release_block.hpp>

namespace px {

	template <typename T>
	class uq_ptr final
	{
	public:
		typedef T element_type;

	public:
		void release() noexcept
		{
			uq_ptr().swap(*this);
		}
		void reset(element_type * pointer, poly_release_block * control_block) noexcept
		{
			uq_ptr(pointer, control_block).swap(*this);
		}
		void swap(uq_ptr & other) noexcept
		{
			std::swap(ptr, other.ptr);
			std::swap(ctrl, other.ctrl);
		}

		element_type * get() const noexcept
		{
			return ptr;
		}
		poly_release_block * control() const noexcept
		{
			return ctrl;
		}
		explicit operator bool() const noexcept
		{
			return ptr != nullptr;
		}
		element_type * operator->() const noexcept
		{
			return ptr;
		}
		typename std::add_lvalue_reference<T>::type operator*() const
		{
			return *ptr;
		}

		// ownership released (transferred to returned value) after call
		template <typename U>
		uq_ptr<U> downcast() noexcept
		{
			U * obj = static_cast<U*>(ptr);
			ptr = nullptr;
			return uq_ptr<U>(obj, ctrl);
		}

		// ownership released (transferred to returned value) after call
		// object released, if cast not succeed
		template <typename U>
		uq_ptr<U> upcast() noexcept
		{
			U * obj = dynamic_cast<U*>(ptr);
			if (!obj) release(); // cast unsuccessful, obect destructed here
			ptr = nullptr;
			return uq_ptr<U>(obj, ctrl);
		}

	public:
		~uq_ptr()
		{
			if (ptr) {
				ctrl->release();
			}
		}
		uq_ptr(element_type * pointer, poly_release_block * control_block) noexcept
			: ptr(pointer)
			, ctrl(control_block)
		{
		}
		uq_ptr() noexcept
			: uq_ptr(nullptr, nullptr)
		{
		}

		// copy, move

		uq_ptr(uq_ptr const& other) = delete;
		uq_ptr & operator=(uq_ptr const& other) = delete;

		uq_ptr(uq_ptr && other) noexcept
			: uq_ptr()
		{
			other.swap(*this);
		}
		uq_ptr & operator=(uq_ptr && other)
		{
			other.swap(*this);
			return *this;
		}

	private:
		element_type *			ptr;
		poly_release_block *	ctrl;
	};

	template<typename T, typename U> inline bool operator==(uq_ptr<T> const& a, uq_ptr<U> const& b) { return a.get() == b.get(); }
	template<typename T, typename U> inline bool operator!=(uq_ptr<T> const& a, uq_ptr<U> const& b) { return a.get() != b.get(); }
	template<typename T, typename U> inline bool operator==(uq_ptr<T> const& a, U const* b) { return a.get() == b; }
	template<typename T, typename U> inline bool operator!=(uq_ptr<T> const& a, U const* b) { return a.get() != b; }
	template<typename T, typename U> inline bool operator==(T const* a, uq_ptr<U> const& b) { return a == b.get(); }
	template<typename T, typename U> inline bool operator!=(T const* a, uq_ptr<U> const& b) { return a != b.get(); }
	template<typename T> inline bool operator<(uq_ptr<T> const& a, uq_ptr<T> const& b) { return std::less<T*>()(a.get(), b.get()); }

}