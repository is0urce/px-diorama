// name: colors.hpp
// type: c++ header
// desc: color defaults
// auth: is0urce

#pragma once

#include "color.hpp"

namespace px {
	namespace colors {

		namespace {
			const static color black = color(0, 0, 0);
			const static color white = color(1, 1, 1);

			const static color red = color(1, 0, 0);
			const static color green = color(0, 1, 0);
			const static color blue = color(0, 0, 1);

			const static color transparent = color({}, {}, {}, 0);
		}
	}
}