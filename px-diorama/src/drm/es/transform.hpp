#pragma once

#include <px/common/point.hpp>

namespace px
{
	struct transform
	{
	public:
		void move(point2 const& movement) noexcept
		{
			current += movement;
		}
		void place(point2 position) noexcept
		{
			current = position;
		}
		auto const& position() const noexcept
		{
			return current;
		}
		auto x() const noexcept
		{
			return current.x();
		}
		auto y() const noexcept
		{
			return current.y();
		}

	private:
		point2 current;
	};
}