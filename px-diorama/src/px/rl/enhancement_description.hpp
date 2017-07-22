// name: enhancement_description.hpp
// type: c++ header
// desc: enum
// auth: is0urce

// status types

#pragma once

#include <string>
#include <px/rl/enhancement_status.hpp>

namespace px {
	namespace rl {

		struct enhancement_description
		{
			std::string name;
			std::string value;
			enhancement_status status;
		};
	}
}