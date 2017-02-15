// name: container_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/es/basic_system.hpp>
#include "container_component.hpp"

namespace px {
	namespace es {
		class container_system final
			: public basic_system<container_component, 1000>
		{
		};
	}
}