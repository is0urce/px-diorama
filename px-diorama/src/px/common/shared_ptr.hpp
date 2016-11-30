// name: shared_ptr.hpp
// type: c++ header
// auth: is0urce

#pragma once

// shared ptr with explicit control block allocation

#include <atomic>
#include <memory> // for default deleter

namespace px
{
	template<typename T>
	class base_ctrl_block
	{
	public:
		void increment() noexcept
		{
			++m_counter;
		}
		unsigned int decrement() noexcept
		{
			return --m_counter;
		}
		unsigned int counter() const noexcept
		{
			return m_counter;
		}
		void release(T * ptr)
		{
			release_pointer(ptr);
		}

	public:
		base_ctrl_block()
			: m_counter(0)
		{
		}
		virtual ~base_ctrl_block()
		{
		}
	protected:
		virtual void release_pointer(T * /*ptr*/) = 0;

	private:
		std::atomic<unsigned int> m_counter;
	};

	template<typename T, typename Deleter = std::default_delete<T>>
	class ctrl_block
		: public base_ctrl_block<T>
	{
	public:
		ctrl_block()
			: m_deleter(Deleter{})
		{
		}
		ctrl_block(Deleter d)
			: m_deleter(d)
		{
		}
		ctrl_block(ctrl_block && that)
			: ctrl_block()
		{
			std::swap(m_deleter, that.m_deleter);
		}
		ctrl_block& operator=(ctrl_block && that)
		{
			std::swap(m_deleter, that.m_deleter);
			return *this;
		}
		virtual ~ctrl_block()
		{
		}

	protected:
		virtual void release_pointer(T * ptr) override
		{
			m_deleter(ptr);
		}

	private:
		Deleter m_deleter;
	};

	template<typename T, typename Deleter = std::default_delete<T>>
	class heap_ctrl_block
		: public base_ctrl_block<T>
	{
	public:
		heap_ctrl_block()
			: m_deleter(Deleter{})
		{
		}
		heap_ctrl_block(Deleter d)
			: m_deleter(d)
		{
		}
		virtual ~heap_ctrl_block()
		{
		}

	protected:
		virtual void release_pointer(T * ptr) override
		{
			m_deleter(ptr);
			delete this;
		}

	private:
		Deleter m_deleter;
	};

	template <typename T>
	class shared_ptr
	{
	public:
		void swap(shared_ptr & rhs)
		{
			std::swap(m_pointer, rhs.m_pointer);
			std::swap(m_ctrl, rhs.m_ctrl);
		}

		T * get() const noexcept
		{
			return m_pointer;
		}
		void reset() noexcept
		{
			shared_ptr().swap(*this);
		}
		void reset(T * ptr)
		{
			shared_ptr(ptr).swap(*this);
		}
		void reset(T * ptr, base_ctrl_block<T> * ctrl)
		{
			shared_ptr(ptr, ctrl).swap(*this);
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
		base_ctrl_block<T> * block() const noexcept
		{
			return m_ctrl;
		}
		bool unique() const noexcept
		{
			return m_pointer && m_ctrl->counter() == 1;
		}

	public:
		shared_ptr() noexcept
			: m_pointer(nullptr)
		{
		}
		shared_ptr(T * ptr, base_ctrl_block<T> * ctrl) noexcept
			: m_pointer(ptr)
			, m_ctrl(ctrl)
		{
			if (m_pointer) m_ctrl->increment();
		}
		shared_ptr(T * ptr)
			: shared_ptr(ptr, new heap_ctrl_block<T>())
		{
		}
		shared_ptr(shared_ptr const& rhs) noexcept
			: shared_ptr(rhs.get(), rhs.m_ctrl)
		{
		}
		shared_ptr(shared_ptr && rhs) noexcept
			: shared_ptr()
		{
			swap(rhs);
		}
		shared_ptr & operator=(shared_ptr && rhs) noexcept
		{
			swap(rhs);
			return *this;
		}
		shared_ptr & operator=(shared_ptr const& rhs) noexcept
		{
			shared_ptr(rhs).swap(*this);
			return *this;
		}
		shared_ptr & operator=(T * rhs) noexcept
		{
			shared_ptr(rhs).swap(*this);
			return *this;
		}

		// downcast
		template <typename Sub>
		shared_ptr(shared_ptr<Sub> const& rhs) noexcept
			: shared_ptr(rhs.get(), reinterpret_cast<base_ctrl_block<T>*>(rhs.block()))
		{
		}
		template <typename Sub>
		shared_ptr & operator=(shared_ptr<Sub> const& rhs) noexcept
		{
			shared_ptr(rhs).swap(*this);
			return *this;
		}

		~shared_ptr()
		{
			if (m_pointer && m_ctrl->decrement() == 0)
			{
				m_ctrl->release(m_pointer);
			}
		}

	private:
		T * m_pointer;
		base_ctrl_block<T> * m_ctrl;
	};

	template<typename T, typename U> inline bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() == b.get(); }
	template<typename T, typename U> inline bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() != b.get(); }
	template<typename T, typename U> inline bool operator==(shared_ptr<T> const& a, U * b) { return a.get() == b; }
	template<typename T, typename U> inline bool operator!=(shared_ptr<T> const& a, U * b) { return a.get() != b; }
	template<typename T, typename U> inline bool operator==(T * a, shared_ptr<U> const& b) { return a == b.get(); }
	template<typename T, typename U> inline bool operator!=(T * a, shared_ptr<U> const& b) { return a != b.get(); }

	template<typename T> inline bool operator<(shared_ptr<T> const & a, shared_ptr<T> const & b)
	{
		return std::less<T*>()(a.get(), b.get());
	}
	template<typename T> void swap(shared_ptr<T> & lhs, shared_ptr<T> & rhs)
	{
		lhs.swap(rhs);
	}
	template<typename T, typename Super>
	shared_ptr<T> dynamic_pointer_cast(shared_ptr<Super> const & r)
	{
		return shared_ptr<T>(dynamic_cast<T*>(r.get()), reinterpret_cast<base_ctrl_block<T>*>(r.block()));
	}

	template<typename T, typename Join>
	struct ctrl_block_pack
	{
	public:
		T value;
		Join block;
	public:
		template<typename... Args>
		ctrl_block_pack(Args... args)
			: value(std::forward<Args>(args)...)
		{
		}
	};
	template<typename T, typename... Args>
	shared_ptr<T> make_shared(Args&&... args)
	{
		auto * ptr = new ctrl_block_pack<T, ctrl_block<T>>(std::forward<Args>(args)...);
		return{ &ptr->value, &ptr->block };
	}
}