// name: storage_component.cpp
// type: c++ code
// auth: is0urce
// desc: class methods implementation

#include "storage_component.hpp"

#include "drm/environment.hpp"

namespace px {

	storage_component::~storage_component()
	{
	}

	storage_component::storage_component()
	{
	}

	bool storage_component::is_useable(body_component const* /*user*/, environment const& /*shell*/) const
	{
		return true;
	}

	void storage_component::use_useable(body_component * /*user*/, environment & shell)
	{
		shell.expose_inventory(linked<container_component>());
	}
}