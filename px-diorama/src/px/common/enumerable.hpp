// name: enumerable.hpp
// type: c++ header
// desc: enumerable wrapper
// auth: is0urce

#pragma once

namespace px {

	template <typename Container>
	class enumerable
		: public Container
	{
	public:
		template <typename UnaryOperator>
		void enumerate(UnaryOperator && fn) const
		{
			for (auto const& item : *this) {
				fn(item);
			}
		}
		template <typename UnaryOperator>
		void enumerate(UnaryOperator && fn)
		{
			for (auto & item : *this) {
				fn(item);
			}
		}
	};
}