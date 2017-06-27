// name: stream.hpp
// type: c++
// auth: is0urce
// desc: template class

#pragma once

#include <px/common/assert.hpp>

#include <thread>
#include <stdexcept>

namespace px {

	template <typename Data>
	class stream final
	{
	public:
		typedef Data data_type;

	public:
		bool loaded() const noexcept
		{
			return m_loaded;
		}
		void wait()
		{
			finish_loading();
			exception_check();
		}
		template <typename Operator>
		void load(Operator && fn)
		{
			wait();
			fn(m_data);
		}
		template <typename Operator>
		void load_stream(Operator && fn)
		{
			wait();
			m_loaded = false;
			// function closure taken by value
			m_thread = std::thread([this, fn]() {
				try
				{
					fn(m_data);
					m_loaded = true;
				}
				catch (...)
				{
					exc_ptr = std::current_exception();
				}
			});
		}
		data_type const* operator->() const
		{
			return &m_data;
		}
		data_type * operator->()
		{
			return &m_data;
		}
		data_type const* get() const
		{
			return &m_data;
		}
		data_type * get()
		{
			return &m_data;
		}
		data_type const& userdata() const
		{
			return m_data;
		}
		data_type & userdata()
		{
			return m_data;
		}

	public:
		~stream()
		{
			finish_loading();
		}
		stream()
			: m_loaded(false)
		{
		}
		stream(stream const&) = delete;
		stream & operator=(stream const&) = delete;

	private:
		void finish_loading()
		{
			if (m_thread.joinable()) {
				m_thread.join();
			}
			m_loaded = true;
		}
		void exception_check() const
		{
			if (exc_ptr) {
				try
				{
					std::rethrow_exception(exc_ptr);
				}
				catch (std::exception const& exc)
				{
					throw std::runtime_error(std::string("px::map_stream - std::exception in loading map thread; what = ") + exc.what());
				}
			}
		}

	private:
		data_type			m_data;
		volatile bool		m_loaded;	// flag set is data loaded
		std::thread			m_thread;
		std::exception_ptr	exc_ptr;
	};
}
