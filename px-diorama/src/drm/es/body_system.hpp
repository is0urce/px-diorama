// name: body_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/es/basic_system.hpp>

#include "drm/es/body_component.hpp"

namespace px {
	namespace es {

		class body_system final
			: public basic_system<body_component>
		{
		};
	}
}