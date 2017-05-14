// name: popup.hpp

#pragma once

#include <px/common/color.hpp>
#include <string>

namespace px {

	struct popup
	{
		float x;
		float y;
		std::string text;
		color tint;
	};
}