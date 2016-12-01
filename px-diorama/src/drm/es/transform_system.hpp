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
			result->place(position);
			return result;
		}

	private:
		pool_chain<transform_component, 1000> m_pool;
	};
}