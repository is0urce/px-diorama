// name: logger.hpp

#pragma once

#include <ostream>
#include <fstream>
#include <mutex>
#include <string>

namespace px
{
	class logger
	{
	public:
		void write(std::string const& text)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			open();
			m_stream << text.c_str() << std::endl;
			m_stream.flush();
		}

	public:
		logger(std::string file)
			: m_name(file), m_opened(false)
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
			if (!m_opened)
			{
				m_stream.open(m_name, std::ios::out | std::ios::app);
			}
		}
		void close()
		{
			m_stream.close();
			m_opened = false;
		}

	private:
		std::string m_name;
		bool m_opened;
		std::ofstream m_stream;
		std::mutex m_mutex;
	};
}