// name: timer.hpp
// type: c++ header
// desc: class declaration & implementation

// time track

#pragma once

namespace px
{
	template <typename QueryOperator>
	class timer
	{
	private:
		QueryOperator m_query;
	public:
		typedef decltype(m_query()) precision_type;
	private:
		precision_type m_start;

	public:
		timer(QueryOperator const& query = QueryOperator{})
			: m_query(query)
		{
			restart();
		}

		void restart()
		{
			m_start = m_query();
		}
		void restart(precision_type time)
		{
			m_start = time;
		}

		// time in seconds
		auto measure() const
		{
			return m_query() - m_start;
		}
		auto measure()
		{
			return m_query() - m_start;
		}

		auto operator()() const
		{
			return measure();
		}
		auto operator()()
		{
			return measure();
		}
	};
}