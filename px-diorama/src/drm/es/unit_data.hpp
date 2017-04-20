// name: unit_data.hpp
// type: c++ header
// desc: struct
// auth: is0urce

#pragma once

#include "unit.hpp"

namespace px {

	struct unit_data
	{
		bool transform;
		bool body;

		template <typename Archive>
		void serialize(Archive & archive)
		{
			archive(transform, body);
		}
	};
}