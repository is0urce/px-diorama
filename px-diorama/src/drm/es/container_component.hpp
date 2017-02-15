// name: container_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "drm/rl/container.hpp"

namespace px {
	class transform_component;

	class container_component final
		: public rl::container
		, public es::component
		, public es::link_dispatcher<container_component>
		, public es::link<transform_component>
	{
	public:
		virtual ~container_component()
		{
		}
	};
}