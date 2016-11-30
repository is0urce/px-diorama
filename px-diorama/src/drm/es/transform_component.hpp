// name: transform_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include "transform.hpp"
#include <px/es/component.hpp>
#include <px/es/link.hpp>

namespace px {
	class transform_component
		: public transform
		, public es::component
		, link_dispatcher<transform_component>
	{
	};
}