// name: shared_ptr.hpp
// type: c++ header
// auth: is0urce

#pragma once

// shared ptr with explicit control block allocation

#include <atomic>
#include <memory> // for default deleter

namespace px {

	template <typename T>
	class default_delete
	{
	public:
		void operator()(T const* ptr)
		{
			delete ptr;
		}
	};

	template<typename T>
	class base_control_block
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
		void release()
		{
			release_pointer(m_memory);
		}
		//void assign_memory(T * memory)
		//{
		//	m_memory = memory;
		//}
		void swap(base_control_block & that)
		{
			m_counter = that.m_counter.exchange(m_counter);
			std::swap(m_memory, that.m_memory);
		}

	public:
		virtual ~base_control_block()
		{
		}
		base_control_block(T * memory) noexcept
			: m_counter(0)
			, m_memory(memory)
		{
		}
		base_control_block() noexcept
			: m_counter(0)
			, m_memory(nullptr)
		{
		}
		//base_control_block(base_control_block const&) = default;
		//base_control_block& operator=(base_control_block const&) = default;
	protected:
		virtual void release_pointer(T * /*ptr*/) = 0;

	private:
		std::atomic<unsigned int> m_counter;
		T * m_memory;
	};

	template<typename T, typename Deleter = default_delete<T>>
	class control_block
		: public base_control_block<T>
	{
	public:
		void swap(control_block & that)
		{
			std::swap(m_deleter, that.m_deleter);
			base_control_block::swap(that);
		}
	public:
		virtual ~control_block()
		{
		}
		control_block()
			: base_control_block(nullptr)
			, m_deleter(Deleter{})
		{
		}
		control_block(T * memory)
			: base_control_block(memory)
			, m_deleter(Deleter{})
		{
		}
		control_block(T * memory, Deleter d)
			: base_control_block(memory)
			, m_deleter(d)
		{
		}
		control_block(control_block && that)
			: ctrl_block()
		{
			swap(that);
		}
		control_block& operator=(control_block && that)
		{
			swap(that);
			return *this;
		}
		//control_block(control_block const&) = default;
		//control_block& operator=(control_block const&) = default;

	protected:
		virtual void release_pointer(T * ptr) override
		{
			m_deleter(ptr);
		}

	private:
		Deleter m_deleter;

	public:
		struct join_pack
		{
		public:
			T value;
			control_block block;
		public:
			template <typename... Args>
			join_pack(Args... args)
				: value(std::forward<Args>(args)...)
				, block(&value)
			{
			}
		};
	};

	//template<typename T, typename Deleter = std::default_delete < T >>
	//class dissolve_control_block
	//	: public base_control_block<T>
	//{
	//public:
	//	dissolve_control_block(T * memory)
	//		: base_control_block(memory)
	//		, m_deleter(Deleter{})
	//	{
	//	}
	//	dissolve_control_block(T * memory, Deleter d)
	//		: base_control_block(memory)
	//		, m_deleter(d)
	//	{
	//	}
	//	virtual ~dissolve_control_block()
	//	{
	//	}

	//protected:
	//	virtual void release_pointer(T * ptr) override
	//	{
	//		m_deleter(ptr);
	//		delete this;
	//	}

	//private:
	//	Deleter m_deleter;
	//};

	//template<typename T>
	//class dissolve_pack_block
	//	: public base_control_block<T>
	//{
	//public:
	//	struct join_pack;

	//public:
	//	virtual ~dissolve_pack_block()
	//	{
	//	}
	//	dissolve_pack_block(join_pack * pack)
	//	{
	//	}

	//protected:
	//	virtual void release_pointer(T * ptr) override
	//	{
	//		delete m_pack;
	//	}

	//private:
	//	join_pack * m_pack;

	//public:
	//	struct join_pack
	//	{
	//	public:
	//		T value;
	//		dissolve_pack_block block;
	//	public:
	//		template <typename... Args>
	//		join_pack(Args... args)
	//			: value(std::forward<Args>(args)...)
	//		{
	//		}
	//	};
	//};

	template<typename T>
	class shared_ptr final
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
		void reset(T * ptr, base_control_block<T> * ctrl)
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
		base_control_block<T> * block() const noexcept
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
		shared_ptr(T * ptr, base_control_block<T> * ctrl) noexcept
			: m_pointer(ptr)
			, m_ctrl(ctrl)
		{
			if (m_pointer) m_ctrl->increment();
		}
		shared_ptr(T * ptr)
			: shared_ptr(ptr, new dissolve_control_block<T>())
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
			: shared_ptr(rhs.get(), reinterpret_cast<base_control_block<T>*>(rhs.block()))
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
				m_ctrl->release();
			}
		}

	private:
		T * m_pointer;
		base_control_block<T> * m_ctrl;
	};

	template<typename T, typename U> inline bool operator==(shared_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() == b.get(); }
	template<typename T, typename U> inline bool operator!=(shared_ptr<T> const& a, shared_ptr<U> const& b) { return a.get() != b.get(); }
	template<typename T, typename U> inline bool operator==(shared_ptr<T> const& a, U * b) { return a.get() == b; }
	template<typename T, typename U> inline bool operator!=(shared_ptr<T> const& a, U * b) { return a.get() != b; }
	template<typename T, typename U> inline bool operator==(T * a, shared_ptr<U> const& b) { return a == b.get(); }
	template<typename T, typename U> inline bool operator!=(T * a, shared_ptr<U> const& b) { return a != b.get(); }

	template<typename T> inline bool operator<(shared_ptr<T> const& a, shared_ptr<T> const& b)
	{
		return std::less<T*>()(a.get(), b.get());
	}
	template<typename T> void swap(shared_ptr<T> & lhs, shared_ptr<T> & rhs)
	{
		lhs.swap(rhs);
	}
	template<typename T, typename Super>
	shared_ptr<T> dynamic_pointer_cast(shared_ptr<Super> const& r)
	{
		return shared_ptr<T>(dynamic_cast<T*>(r.get()), reinterpret_cast<base_control_block<T>*>(r.block()));
	}

	template<typename T, typename... Args>
	shared_ptr<T> make_shared(Args&&... args)
	{
		auto * pack = new control_block<T>::join_pack(std::forward<Args>(args)...);

		return shared_ptr<T>(&(pack->value), &(pack->block));
	}
}