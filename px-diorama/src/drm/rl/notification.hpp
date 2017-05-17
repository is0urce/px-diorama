// name: notification.hpp

#pragma once

#include <px/common/color.hpp>
#include <px/common/point.hpp>

#include <string>

namespace px {

	struct notification
	{
		std::string		text;		// message
		color			tint;		// color of text
		float			size;		// size of font
	};
}