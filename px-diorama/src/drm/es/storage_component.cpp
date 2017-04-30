// name: storage_component.cpp
// type: c++ code
// auth: is0urce
// desc: class methods implementation

#pragma once

#include "storage_component.hpp"

#include "drm/environment.hpp"

#include "body_component.hpp"
#include "container_component.hpp"

namespace px {

	storage_component::~storage_component()
	{
	}

	storage_component::storage_component()
	{
	}

	bool storage_component::is_useable(body_component const* user, environment const& /*shell*/) const
	{
		auto user_pawn = user ? user->linked<transform_component>() : nullptr;
		auto this_pawn = linked<container_component, transform_component>();

		return user_pawn && this_pawn
			&& point2::king_distance(user_pawn->position(), this_pawn->position()) <= 1;
	}

	void storage_component::use_useable(body_component * /*user*/, environment & shell)
	{
		shell.expose_inventory(linked<container_component>());
	}
}