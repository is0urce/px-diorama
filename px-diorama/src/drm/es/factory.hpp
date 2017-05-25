// name: factory.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

#pragma once

// collection of systems
// instantiated at once
// takes a lot of space, should be part of heap allocation (or kindly ask os for more stack memory)

#include "transform_system.hpp"
#include "sprite_system.hpp"
#include "body_system.hpp"
#include "container_system.hpp"

#include "character_system.hpp"

#include "storage_component.hpp"
#include "player_component.hpp"
#include "workshop_component.hpp"
#include "character_component.hpp"
#include "npc_system.hpp"

#include <string>

namespace px {

	class factory
	{
	public:
		auto make_transform(point2 location)
		{
			return m_transforms.make_shared(location);
		}
		auto make_sprite(std::string const& name)
		{
			return m_sprites.make_shared(name);
		}
		auto make_body()
		{
			return m_bodies.make_shared();
		}
		auto make_container()
		{
			return m_containers.make_shared();
		}
		auto make_character()
		{
			return m_characters.make_shared();
		}
		auto make_storage()
		{
			return px::make_shared<storage_component>();
		}
		auto make_player()
		{
			return px::make_shared<player_component>();
		}
		auto make_workshop()
		{
			return px::make_shared<workshop_component>();
		}
		auto make_npc()
		{
			return m_npc.make_shared();
		}

		auto sprites()
		{
			return &m_sprites;
		}
		auto transforms()
		{
			return &m_transforms;
		}
		auto characters()
		{
			return &m_characters;
		}
		auto npc()
		{
			return &m_npc;
		}

	public:
		factory()
		{
		}
		factory(factory const&) = delete;
		factory& operator=(factory const&) = delete;

	private:
		es::transform_system	m_transforms;
		es::sprite_system		m_sprites;
		es::body_system			m_bodies;
		es::container_system	m_containers;
		es::character_system	m_characters;
		es::npc_system			m_npc;
	};
}