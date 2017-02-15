// name: basic_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/common/pool_chain.hpp>

#include <memory>

namespace px {
	namespace es {

		template <typename Component, unsigned int Size>
		class basic_system
		{
		public:
			typedef pool_chain<Component, Size> pool_type;

		public:
			auto make_shared()
			{
				auto result = m_pool->make_shared();
				return result;
			}

		public:
			basic_system()
				: m_pool(std::make_unique<pool_type>())
			{
			}

		private:
			std::unique_ptr<pool_type> m_pool;
		};
	}
}