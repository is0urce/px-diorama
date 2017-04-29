// name: workshop_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "useable.hpp"

namespace px {

	class workshop_component final
		: public useable
		, public es::component
		, public es::link_dispatcher<workshop_component>
		, public es::link<body_component>
	{
	public:
		virtual ~workshop_component()
		{
		}
		workshop_component()
			: m_workshop(0)
		{
		}

	protected:
		virtual bool is_useable(body_component const* /*user*/, environment const& /*shell*/) const override
		{
			return true;
		}
		virtual void use_useable(body_component * /*user*/, environment & /*shell*/) override
		{
		}

	private:
		unsigned int m_workshop;
	};
}