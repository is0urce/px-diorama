// name: transform_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

// both transform and hierarchy

#include <px/es/transform.hpp>

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>
#include <px/es/component_collection.hpp>

namespace px {
	class transform_component final
		: public es::transform<transform_component>
		, public es::component
		, public es::link_dispatcher<transform_component>
		, public es::component_collection
	{
	public:
		transform_component()
		{
		}
		virtual ~transform_component()
		{
			retract();
			reincarnate();
		}

	protected:
		virtual void activate_component() override
		{
			insert();
		}
		virtual void deactivate_component() override
		{
			retract();
		}
	};
}