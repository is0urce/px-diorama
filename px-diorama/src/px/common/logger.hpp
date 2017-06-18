// name: logger.hpp

#pragma once

#include <ostream>
#include <fstream>
#include <mutex>
#include <string>

namespace px {

	class logger
	{
	public:
		void write_line(std::string const& text)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			open();
			m_stream << text.c_str() << std::endl;
			m_stream.flush();
		}

	public:
		logger(std::string file)
			: m_name(file)
		{
		}
		logger() : logger("log.txt")
		{
		}
		~logger()
		{
			close();
		}

	private:
		void open()
		{
			if (!m_stream.is_open()) {
				m_stream.open(m_name, std::ios_base::out | std::ios_base::app);
			}
		}
		void close()
		{
			m_stream.close();
		}

	private:
		std::string m_name;
		std::ofstream m_stream;
		std::mutex m_mutex;
	};
}