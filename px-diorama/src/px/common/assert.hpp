// name: assert.hpp
// type: c++ header
// desc: assert macro

#pragma once

#ifdef NDEBUG

#define px_assert(EXPRESSION) ((void)0)
#define px_assert_nothrow(EXPRESSION) ((void)EXPRESSION);

#else

#include <stdexcept>
#include <string>
#include <sstream>

inline void px_debug_assert(const char* expression, const char* file, int line)
{
	std::stringstream ss;
	ss << expression << " in " << file << " line #" << std::to_string(line);
	throw std::runtime_error(ss.str());
}

#define px_assert(EXPRESSION) ((EXPRESSION) ? (void)0 : px_debug_assert(#EXPRESSION, __FILE__, __LINE__))

#define px_assert_nothrow(EXPRESSION) (px_debug_assert((EXPRESSION).what(), __FILE__, __LINE__))

#endif