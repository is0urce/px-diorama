// name: pool_chain.hpp
// type: c++ header
// desc: template class
// auth: is0urce

#pragma once

// pool chain is a chain of pools
// if root pool run out, new pools requested from heap

#include <px/common/pool.hpp>

#include <array>
#include <memory>

namespace px
{
	template <typename T, size_t ChunkSize>
	class pool_chain
	{
	public:
		struct node;
		struct smart_deleter;

	public:
		typedef pool<T, ChunkSize> pool_type;
		typedef T element;
		typedef T* pointer;
		typedef std::unique_ptr<T, smart_deleter> unique_ptr;
		typedef shared_ptr<T> shared_ptr;

	public:
		size_t size() const noexcept
		{
			size_t result = 0;
			for (node const* i = &m_root; i != nullptr; i = i->next.get())
			{
				result += i->chunk.size();
			}
			return result;
		}
		bool empty() const noexcept
		{
			return size() == 0;
		}
		// exposed for debug / performance queries, always has at least 1
		size_t chunks() const noexcept
		{
			return m_chunks;
		}

		// check only range of pointer, not correctness (i.e alignment)
		bool contains(T const* ptr) const noexcept
		{
			for (node const* i = &m_root; i != nullptr; i = i->next.get())
			{
				if (i->chunk.contains(ptr)) return true;
			}
			return false;
		}

		template <typename... Args>
		T* request(Args... args)
		{
			//++m_count;
			return aquire_free_pool().request(std::forward<Args>(args)...);
		}
		template <typename... Args>
		auto make_shared(Args... args)
		{
			//++m_count;
			return aquire_free_pool().make_shared(std::forward<Args>(args)...);
		}
		template <typename... Args>
		unique_ptr make_unique(Args... args)
		{
			//++m_count;
			auto &chunk = aquire_free_pool();
			return{ chunk.request(std::forward<Args>(args)...) , { this, &chunk } };
		}
		template <typename... Args>
		std::shared_ptr<T> make_std(Args... args)
		{
			//++m_count;
			return aquire_free_pool().make_std(std::forward<Args>(args)...);
		}

		// it's safe to release already released objects
		void release(T* ptr)
		{
			for (node* i = &m_root; i != nullptr; i = i->next.get())
			{
				if (i->chunk.contains(ptr))
				{
					if (i->chunk.release(ptr))
					{
//						--m_count;
					}
					return;
				}
			}
		}

		template <typename Operator>
		void enumerate(Operator && op)
		{
			for (node* i = &m_root; i != nullptr; i = i->next.get())
			{
				i->chunk.enumerate(std::forward<Operator>(op));
			}
		}
		template <typename Operator>
		void enumerate(Operator && op) const
		{
			for (node const* i = &m_root; i != nullptr; i = i->next.get())
			{
				i->chunk.enumerate(std::forward<Operator>(op));
			}
		}

		void clear() noexcept
		{
			m_root.chunk.clear();
			m_root.next.reset();
//			m_count = 0;
			m_chunks = 1;
		}

		// remove unused chunks
		// don't actualy make pool chain ''optimal''
		// just frees currently unused extra memory
		// chain always has at least one root chunk after invoke even it's empty
		void optimize() noexcept
		{
			for (std::unique_ptr<node>* i = &m_root.next; *i; i = &(*i)->next)
			{
				if ((*i)->chunk.size() == 0)
				{
					std::swap(*i, (*i)->next);
					--m_chunks;
				}
			}
		}

	public:
		pool_chain() noexcept
			: m_chunks(1)
//			, m_count(0)
		{
		}
		pool_chain(pool_chain const&) = delete;
		pool_chain& operator=(pool_chain const&) = delete;

	private:
		pool_type& aquire_free_pool()
		{
			for (node* i = &m_root; true; i = i->next.get())
			{
				if (!i->chunk.full())
				{
					return i->chunk;
				}
				if (i->next == nullptr)
				{
					i->next = std::make_unique<node>();
					++m_chunks;
				}
			}
		}


	private:
		node m_root;
		//size_t m_count; // cashed for fast size query
		size_t m_chunks; // current chunks

	public:
		struct node
		{
			pool_type chunk;
			std::unique_ptr<node> next;
		};
		struct smart_deleter
		{
		public:
			void operator()(T* ptr) // lambda
			{
				m_chunk->release(ptr);
//				--m_chain->m_count;
			}
			smart_deleter(pool_chain* current, pool_type* chunk) noexcept
				: m_chain(current), m_chunk(chunk)
			{
			}
			smart_deleter() noexcept
				: smart_deleter(nullptr, nullptr)
			{
			}
		private:
			pool_chain* m_chain;
			pool_type* m_chunk;
		};
	};
}