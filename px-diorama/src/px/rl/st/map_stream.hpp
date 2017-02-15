// name: map_stream.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/common/matrix.hpp>
#include <px/rl/tile.hpp>

#include <thread>
#include <list>
#include <string>
#include <stdexcept>

namespace px
{
	namespace rl
	{
		template <typename TileProps, typename Unit, unsigned int W, unsigned int H>
		class map_stream
		{
		public:
			typedef tile<TileProps> tile_type;
			typedef matrix2<tile_type, W, H> map_type;
			typedef std::list<Unit> unit_list;

		private:
			map_type m_map;
			unit_list m_units;
			volatile bool m_loaded;
			bool m_pending;
			std::thread m_thread;
			std::exception_ptr exc_ptr;

		public:
			map_stream() : m_loaded(true), m_pending(false)
			{
			}
			virtual ~map_stream()
			{
				finish_loading();
			}
			map_stream(const map_stream&) = delete;

		private:
			void finish_loading()
			{
				if (m_thread.joinable())
				{
					m_thread.join();
				}
			}

		public:
			void wait()
			{
				finish_loading();
				exception_check();
			}
			void exception_check()
			{
				if (exc_ptr)
				{
					try
					{
						std::rethrow_exception(exc_ptr);
					}
					catch (const std::exception &e)
					{
						throw std::runtime_error(std::string("px::map_stream - std::exception in loading map thread; what = ") + e.what());
					}
				}
			}
			bool loaded() const
			{
				return m_loaded;
			}

			// is there units for merge
			bool pending() const
			{
				return m_pending;
			}

			template <typename Op>
			void load(Op fn)
			{
				if (m_pending) throw std::runtime_error("px::rl::map_stream::load() - pending");

				m_pending = true;
				fn(m_map, m_units);
			}
			template <typename Op>
			void load_stream(Op fn)
			{
				if (m_pending) throw std::runtime_error("px::rl::map_stream::load_stream() - pending");

				m_loaded = false;
				m_pending = true;
				m_thread = std::thread([fn, this]()
				{
					try
					{
						fn(m_map, m_units);
						m_loaded = true;
					}
					catch (...)
					{
						exc_ptr = std::current_exception();
					}
				});
			}
			void splice_into(unit_list &grand)
			{
				if (!m_pending) throw std::runtime_error("px::rl::map_stream::merge() - no pending");

				wait();

				m_pending = false;

				grand.splice(grand.end(), m_units);
			}

			const map_type* operator->() const
			{
				return &m_map;
			}
		};
	}
}