// name: coordinate_ext.hpp
// type: c++ header
// desc: base template class for points and vectors

#pragma once

#include "coordinate.hpp"

namespace px {

	struct lex_less
	{
		template <typename Vector>
		bool operator()(Vector const& a, Vector const& b) const { return a.lex_less(b); }
	};
}