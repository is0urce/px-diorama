// transform.hpp

#pragma once

#include <px/common/point.hpp>
#include <px/common/qtree.hpp>

namespace px
{
	struct transform
	{
	public:
		typedef qtree<transform*> space_type;

	public:
		auto const& position() const noexcept
		{
			return m_current;
		}
		auto x() const noexcept
		{
			return m_current.x();
		}
		auto y() const noexcept
		{
			return m_current.y();
		}
	public:
		transform()
			: m_space(nullptr)
		{
		}

	protected:
		point2 m_current;
		qtree<transform*> * m_space;
	};
}