// name: string_ext.hpp
// type: c++ header
// desc: string functions
// auth: is0urce

#pragma once

#include <px/common/point.hpp>
#include <px/common/vector.hpp>

#include <string>

namespace px {
	namespace {

		inline std::string to_string(point2 const& p)
		{
			return "[" + std::to_string(p.get<0>()) + ":" + std::to_string(p.get<1>()) + "]";
		}

		inline std::string to_string(vector2 const& p)
		{
			return "[" + std::to_string(p.get<0>()) + ":" + std::to_string(p.get<1>()) + "]";
		}
	}
}