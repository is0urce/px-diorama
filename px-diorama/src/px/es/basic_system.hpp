// name: basic_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

// base for multi-element systems
// not an actual base class (but can be used as such)
// but an template and multipurpose placeholder
// also can be used for plain simple systems with no additional behavior

#pragma once

#include <px/common/pool_chain.hpp>

namespace px {
	namespace es {

		// Component - type of elements
		// Size - size of object pools
		template <typename Component, unsigned int Size = 10000>
		class basic_system
		{
		public:
			typedef pool_chain<Component, Size> pool_type;
			typedef typename pool_type::shared_ptr shared_ptr;
			typedef typename pool_type::std_ptr std_ptr;
			typedef typename pool_type::unique_ptr unique_ptr;

		public:
			shared_ptr make_shared()
			{
				return m_pool.make_shared();
			}
			unique_ptr make_unique()
			{
				return  m_pool.make_unique();
			}
			std_ptr make_std()
			{
				return m_pool.make_std();
			}
			template <typename Operator>
			void enumerate(Operator && op) const
			{
				m_pool.enumerate(std::forward<Operator>(op));
			}
			void clear()
			{
				m_pool.clear();
			}

		public:
			basic_system()
			{
			}

		private:
			pool_type m_pool;
		};
	}
}