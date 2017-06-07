// name: enumerator.hpp
// type: c++ header
// desc: enumerator wrapper
// auth: is0urce

#pragma once

namespace px {

	template <typename Container>
	class enumerator
	{
	public:
		template <typename UnaryOperator>
		void enumerate(UnaryOperator && fn) const
		{
			for (auto const& item : m_container) {
				fn(item);
			}
		}
		template <typename UnaryOperator>
		void enumerate(UnaryOperator && fn)
		{
			for (auto & item : m_container) {
				fn(item);
			}
		}

	public:
		enumerator(Container & container)
			: m_container(container)
		{
		}
	private:
		Container & m_container;
	};
}