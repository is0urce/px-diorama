#pragma once

#include <px/memory/uq_ptr.hpp>
#include <px/memory/delete_release_block.hpp>

namespace px {

	template <typename T, typename... Args>
	uq_ptr<T> make_uq(Args &&... args)
	{
		typedef T element_type;
		class join_block final
		{
		public:
			element_type * get_value() noexcept
			{
				return &element;
			}
			poly_release_block * get_control() noexcept
			{
				return &ctrl;
			}

		public:
			join_block(Args &&... args)
				: element(std::forward<Args>(args)...)
				, ctrl(this)
			{
			}
		private:
			element_type						element;
			delete_release_block<join_block>	ctrl;
		};

		// function body

		join_block * pack = new join_block(std::forward<Args>(args)...);
		return { pack->get_value(), pack->get_control() };
	}
}