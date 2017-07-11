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

#include "npc_system.hpp"

#include "storage_component.hpp"
#include "player_component.hpp"
#include "workshop_component.hpp"
#include "character_component.hpp"
#include "deposit_component.hpp"

#include <string>

namespace px {

	class environment;

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
		auto make_deposit()
		{
			return px::make_shared<deposit_component>();
		}

		es::transform_system * transforms() noexcept
		{
			return &m_transforms;
		}
		es::sprite_system * sprites() noexcept
		{
			return &m_sprites;
		}
		es::body_system * bodies() noexcept
		{
			return &m_bodies;
		}
		es::character_system * characters() noexcept
		{
			return &m_characters;
		}
		es::npc_system * npc() noexcept
		{
			return &m_npc;
		}

		void update(double delta)
		{
			m_sprites.update(delta);
		}
		void fixed_update(unsigned int delta)
		{
			m_bodies.fixed_update(delta);
			m_npc.fixed_update(delta);
		}
		void provide_environment(environment * shell)
		{
			m_bodies.provide_environment(shell);
			m_characters.provide_environment(shell);
			m_npc.provide_environment(shell);
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