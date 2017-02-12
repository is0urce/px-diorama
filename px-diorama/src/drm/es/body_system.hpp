// name: body_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/common/pool_chain.hpp>
#include "body_component.hpp"

#include <memory>

namespace px {
	class body_system final
	{
	public:
		typedef pool_chain<body_component, 1000> pool_type;

	public:
		auto make_shared()
		{
			auto result = m_pool->make_shared();
			return result;
		}

	public:
		body_system()
			: m_pool(std::make_unique<pool_type>())
		{
		}

	private:
		std::unique_ptr<pool_type> m_pool;
	};
}