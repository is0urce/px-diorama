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
		auto make_sprite(std::string name)
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
		auto sprites()
		{
			return m_sprites;
		}
		auto transforms()
		{
			return &m_transforms;
		}

	public:
		factory(es::sprite_system * sprites)
			: m_sprites(sprites)
		{
			if (!sprites) throw std::runtime_error("px::factory::ctor() - sprites is null");
		}
		factory(factory const&) = delete;
		factory& operator=(factory const&) = delete;

	private:
		es::sprite_system * m_sprites;
		es::transform_system m_transforms;
		es::body_system m_bodies;
		es::container_system m_containers;
	};
}