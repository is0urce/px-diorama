// name: sprite.hpp

#pragma once

#include <px/common/color.hpp>

namespace px
{
	struct image
	{
		float sx;
		float sy;
		float dx;
		float dy;
		unsigned int texture;
		unsigned int layer;
		unsigned int glyph;
	};

	struct sprite
		: public image
	{
		color tint;
	};

}