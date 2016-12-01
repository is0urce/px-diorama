#pragma once

#include <px/common/pool_chain.hpp>
#include "transform_component.hpp"

#include <map>

namespace px {
	class transform_system final
	{
	public:
		auto make_shared(point2 position)
		{
			auto result = m_pool.make_shared();
			result->move(position);
			result->incarnate(&m_space);
			return result;
		}

	public:
		transform_system()
			: m_space(64)
		{
		}

	private:
		pool_chain<transform_component, 1000> m_pool;
		transform::space_type m_space;
	};
}