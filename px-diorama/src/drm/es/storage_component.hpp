// name: storage_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include "useable.hpp"

namespace px {

	class container_component;

	class storage_component final
		: public useable
		, public es::component
		, public es::link_dispatcher<storage_component>
		, public es::link<container_component>
	{
	public:
		virtual ~storage_component();
		storage_component();

	protected:
		virtual bool is_useable(body_component const* user, environment const& shell) const override;
		virtual void use_useable(body_component * user, environment & shell) override;
	};
}