// name: transform_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/transform.hpp>
#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

namespace px {
	class body_component;

	class transform_component final
		: public es::transform<transform_component>
		, public es::component
		, public es::link_dispatcher<transform_component>
		, public es::link<body_component>
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