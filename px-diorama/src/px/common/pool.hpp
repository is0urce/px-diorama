// name: pool.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

// object memory pool
// cashe coherent (or at least sequental) enumeration of living objects with On = elements currently allocated
// constant request() and release() methods
// for general performanse there is no internal support for concurrent requests and/or releases

#include <array>
#include <memory>

#include "shared_ptr.hpp"

namespace px {

	template <typename T, size_t Size>
	class pool
	{
	public:
		struct links;
		struct smart_deleter;

	public:
		typedef T element;
		typedef T* pointer;
		typedef shared_ptr<T> shared_ptr; // our
		typedef std::shared_ptr<T> std_ptr; // stl
		typedef std::unique_ptr<T, smart_deleter> unique_ptr;

	public:
		// returns nullptr if all object in pool were requested, full() returns true
		template <typename... Args>
		T* request(Args... args)
		{
			T* result = nullptr;
			links* rec = m_free;
			if (rec != nullptr)
			{
				m_free = m_free->next_free; // pop free stack
				m_live = rec->prev_live == nullptr ? rec : m_live; // set this as root?

				// update links
				if (rec->next_live)
				{
					rec->next_live->prev_live = rec;
				}
				if (rec->prev_live)
				{
					rec->prev_live->next_live = rec;
				}

				// modify aux fields
				rec->live = true;
				++m_current;

				result = reinterpret_cast<T*>(&m_pool[(rec - &m_links[0]) * sizeof(T)]);
				create(*result, std::forward<Args>(args)...);
			}
			return result;
		}

		// pointer must be in correct range and alignment of pool
		// it's safe to release already released objects - it's an nop
		bool release(T * ptr)
		{
			bool flag = false;
			auto index = ptr - reinterpret_cast<T*>(&m_pool[0]);
			if (index >= 0 && index < Size)
			{
				links* rec = &m_links[index];
				if (rec->live) // ensure it's not double release
				{
					flag = true;

					rec->next_free = m_free;
					m_free = rec; // push free stack
					m_live = m_live == rec ? rec->next_live : m_live; // not root anymore?

					// update links
					if (rec->prev_live)
					{
						rec->prev_live->next_live = rec->next_live;
					}
					if (rec->next_live)
					{
						rec->next_live->prev_live = rec->prev_live;
					}

					// modify aux fields
					rec->live = false;
					--m_current;

					destroy(*ptr);
				}
			}
			return flag;
		}

		template <typename... Args>
		shared_ptr make_shared(Args... args)
		{
			T * ptr = request(std::forward<Args>(args)...);
			size_t index = ptr - reinterpret_cast<T*>(&m_pool[0]);
			return{ ptr, &m_links[index].ctrl };
		}
		template <typename... Args>
		unique_ptr make_unique(Args... args)
		{
			return{ request(std::forward<Args>(args)...), this };
		}
		template <typename... Args>
		std_ptr make_std(Args... args)
		{
			T * ptr = request(std::forward<Args>(args)...);
			return{ ptr, smart_deleter(this) };
		}

		size_t size() const noexcept
		{
			return m_current;
		}
		bool full() const noexcept
		{
			return m_current == Size;
		}
		bool empty() const noexcept
		{
			return m_current == 0;
		}
		constexpr static size_t max_size() noexcept
		{
			return Size;
		}

		// checks only range of pointer, not correctness (i.e alignment)
		bool contains(T const* ptr) const noexcept
		{
			return ptr >= reinterpret_cast<T const*>(&m_pool[0])
				&& ptr < reinterpret_cast<T const*>(&m_pool[Size * sizeof(T)]);
		}

		template <typename Operator>
		void enumerate(Operator && op)
		{
			for (links* i = m_live; i != nullptr; i = i->next_live)
			{
				std::forward<Operator>(op)(reinterpret_cast<T&>(m_pool[(i - &m_links[0]) * sizeof(T)]));
			}
		}
		template <typename Operator>
		void enumerate(Operator && op) const
		{
			for (links const* i = m_live; i != nullptr; i = i->next_live)
			{
				std::forward<Operator>(op)(reinterpret_cast<T const&>(m_pool[(i - &m_links[0]) * sizeof(T)]));
			}
		}

		void clear() noexcept
		{
			destroy_existing();
			startup();
		}

	public:
		pool() noexcept
		{
			startup();
		}
		~pool()
		{
			destroy_existing();
		}
		pool(pool const&) = delete;
		pool& operator=(pool const&) = delete;

	private:
		void destroy_existing()
		{
			for (links* i = m_live; i != nullptr; i = i->next_live)
			{
				destroy(reinterpret_cast<T&>(m_pool[(i - &m_links[0]) * sizeof(T)]));
			}
		}
		void startup() noexcept
		{
			m_current = 0;
			m_free = &m_links[0];
			m_live = nullptr;
			for (size_t i = 0; i != Size; ++i)
			{
				m_links[i].next_free = i == Size - 1 ? nullptr : &m_links[i + 1];
				m_links[i].prev_live = i == 0 ? nullptr : &m_links[i - 1];
				m_links[i].next_live = nullptr;
				m_links[i].live = false;
				m_links[i].ctrl = smart_deleter(this);
			}
		}
		template <typename... Args>
		void create(T& item, Args... args)
		{
			new (&item) T(std::forward<Args>(args)...);
		}
		void destroy(T& item)
		{
			item.~T();
			//item; // 'item': unreferenced formal parameter if there is no destructor -> referencing
		}

	public:
		struct smart_deleter
		{
		public:
			void operator()(T * ptr) // lambda
			{
				m_current->release(ptr);
			}
		public:
			smart_deleter(pool * current) noexcept
				: m_current(current)
			{
			}
			smart_deleter() noexcept
				: smart_deleter(nullptr)
			{
			}
			smart_deleter(smart_deleter const& that) noexcept
				: smart_deleter(that.m_current)
			{
			}
		private:
			pool* m_current;
		};
		struct links
		{
			links* next_free;
			links* next_live;
			links* prev_live;
			bool live; // auxiliary state to avoid double released
			control_block<T, smart_deleter> ctrl; // for smart ptrs
		};

	private:
		std::array<char, sizeof(T) * Size> m_pool;
		std::array<links, Size> m_links;
		size_t m_current; // cashed number of living objest for fast size queries
		links* m_free; // first free node (root)
		links* m_live; // first living node (root)
	};
}