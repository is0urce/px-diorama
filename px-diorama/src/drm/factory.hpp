// name: factory.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class


#pragma once

#include "es/transform_system.hpp"
#include "es/sprite_system.hpp"
#include "es/body_system.hpp"
#include "es/container_system.hpp"
#include "es/unit.hpp"

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

	public:
		factory(es::sprite_system * sprites)
			: m_sprites(sprites)
		{
			if (!sprites) throw std::runtime_error("px::factory::ctor() - sprites is null");
		}

	private:
		es::sprite_system * m_sprites;
		es::transform_system m_transforms;
		es::body_system m_bodies;
		es::container_system m_containers;
	};
}