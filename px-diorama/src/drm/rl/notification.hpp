// name: notification.hpp
// type: c++
// auth: is0urce
// desc: struct

#pragma once

#include <px/common/color.hpp>

#include <string>

namespace px {

	struct notification
	{
		std::string		text;		// message
		color			tint;		// color of text
		float			size;		// size of font
	};
}