// name: basic_item.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/loot/stack.hpp>
#include <px/rl/enhancement_collection.hpp>
#include <px/rl/entity.hpp>

namespace px
{
	namespace rl
	{
		template <typename Effect>
		class basic_item
			: public stack
			, public entity
			, public enhancement_collection<Effect>
		{
		public:
			typedef Effect effect_type;
			typedef enhancement_collection<Effect> collection_type;

		public:
			// stack from itm
			bool can_stack(basic_item const& from_item) const noexcept
			{
				return &from_item != this
					&& !full()
					&& static_cast<entity const&>(*this) == static_cast<entity const&>(from_item)
					&& static_cast<collection_type const&>(*this) == static_cast<collection_type const&>(from_item);
			}
			unsigned int try_stack(basic_item & from_item) noexcept
			{
				if (can_stack(from_item)) {
					return stack_from(from_item);
				}
				return from_item.count();
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(static_cast<entity&>(*this));
				archive(static_cast<stack&>(*this));
				archive(static_cast<collection_type&>(*this));
			}
		};
	}
}