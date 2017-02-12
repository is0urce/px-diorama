// name: body_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "drm/rl/inventory.hpp"

namespace px {
	class body_component final
		: public es::component
		, public es::link_dispatcher<body_component>
	{
	public:
		body_component()
		{
		}
		virtual ~body_component()
		{
		}

	protected:
		virtual void activate_component() override
		{
		}
		virtual void deactivate_component() override
		{
		}

	private:
		bool m_inventory;
	};
}