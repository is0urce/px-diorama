// name: item.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/lt/basic_item.hpp>
#include <px/rl/enhancement_collection.hpp>
#include <px/rl/entity.hpp>

#include "drm/rl/effect.hpp"

namespace px
{
	namespace rl
	{
		template <typename Effect>
		class item
			: public basic_item
			, public entity
			, public enhancement_collection<Effect>
		{
		public:
			typedef Effect effect_type;
			typedef enhancement_collection<Effect> collection_type;

		public:
			bool can_stack(item & with) const noexcept
			{
				if (&with == this) return false; // stacking into same item

				if (full()) return false; // no space

				return static_cast<entity>(*this) == static_cast<entity>(with)
					&& static_cast<collection_type>(*this) == static_cast<collection_type>(with);
			}
			unsigned int try_stack(item & with) const noexcept
			{
				if (can_stack(with))
				{
					return stack(with);
				}
				return with.stack();
			}
		};
	}
}