// name: factory.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

#pragma once

#include "transform_system.hpp"
#include "sprite_system.hpp"
#include "body_system.hpp"
#include "container_system.hpp"

#include "storage_component.hpp"
#include "player_component.hpp"

#include <memory>
#include <string>
#include <stdexcept>

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
			return m_sprites->make_shared(name);
		}
		auto make_body()
		{
			return m_bodies.make_shared();
		}
		auto make_container()
		{
			return m_containers.make_shared();
		}
		auto make_storage()
		{
			return px::make_shared<storage_component>();
		}
		auto make_player()
		{
			return px::make_shared<player_component>();
		}

		auto sprites()
		{
			return m_sprites.get();
		}
		auto transforms()
		{
			return &m_transforms;
		}

	public:
		factory()
			: m_sprites(std::make_unique<es::sprite_system>())
		{
		}
		factory(factory const&) = delete;
		factory& operator=(factory const&) = delete;

	private:
		es::transform_system m_transforms;
		std::unique_ptr<es::sprite_system> m_sprites;
		es::body_system m_bodies;
		es::container_system m_containers;
	};
}