// name: sprite.hpp

#pragma once

#include <px/common/color.hpp>

namespace px {

	struct image
	{
		// texture coordinates start/end x/y
		float			sx;
		float			sy;
		float			dx;
		float			dy;

		float			tx;			// center x offset
		float			ty;			// center y offset
		float			mx;			// size x, precalculated
		float			my;			// size y, precalculated

		unsigned int	texture;	// texture atlas index
		unsigned int	layer;		// sorting layer
		unsigned int	glyph;		// alternative glyph

		char const*		name;		// persistent pointer to image name
	};

	struct sprite
		: public image
	{
		color tint;
	};

}