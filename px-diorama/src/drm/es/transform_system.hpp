// name: transform_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/common/pool_chain.hpp>
#include "transform_component.hpp"

#include <map>
#include <memory>

namespace px {
	class transform_system final
	{
	public:
		typedef pool_chain<transform_component, 100000> pool_type;

	public:
		auto make_shared(point2 position)
		{
			auto result = m_pool->make_shared();
			result->place(position);
			result->incarnate(&m_space);
			return result;
		}

	public:
		transform_system()
			: m_pool(std::make_unique<pool_type>())
			, m_space(64)
		{
		}

	private:
		std::unique_ptr<pool_type> m_pool;
		transform_component::space_type m_space;
	};
}