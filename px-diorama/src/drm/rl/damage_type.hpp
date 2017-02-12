// name: damage_type.hpp
// type: c++ header
// desc: enum
// auth: is0urce

#pragma once

namespace px
{
	namespace rl
	{
		enum class damage_type : unsigned int
		{
			pure,

			piercing,
			slashing,
			blunt,
			gravitation,

			arcane,
			curse,
			anomaly,
			disintegration,

			inexplicable,
			sonic,
			magnetic,
			mental,

			fire,
			shock,
			acid,
			ice,

			darkness,
			disease,
			poison,
			light,

			not_valid,
			min_value = pure,
			max_value = not_valid
		};
	}
}