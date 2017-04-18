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

#include <map>
#include <string>

namespace px {

	class factory
	{
	public:
		template <typename Document>
		void start()
		{
		}

		template <typename Blueprint>
		unit & spawn(Blueprint const& blueprint, point2 location)
		{
			unit result;

			px::shared_ptr<sprite_component> sprite_part;
			px::shared_ptr<transform_component> transform_part = m_transforms.make_shared(location);
			px::shared_ptr<body_component> body_part;
			px::shared_ptr<container_component> container_part;

			result.add(transform_part);

			if (auto sprite_data = blueprint.find("sprite") != blueprint.end())
			{
				std::string name = sprite_data["name"];

				sprite_part = m_sprites->make_shared(name);
				result.add(sprite_part);
			}
			if (auto body_data = blueprint.find("body") != blueprint.end())
			{
				body_part = m_bodies->make_shared();
				result.add(body_part);

				//int hp = body_data["hp"];
				//body_part->health().create();
				//body_part->health().set(hp);
			}
			//if (auto sprite_data = blueprint.find("sprite") != blueprint.end())
			//{
			//	sprite_part = m_sprites.make_shared(sprite_data["name"]);
			//	result.add(sprite_part);
			//}

			if (sprite_part && transform_part)
			{
				sprite_part->connect(transform_part.get());
			}
			if (transform_part && body_part)
			{
				transform_part->connect(body_part.get());
			}
			if (body_part && container_part)
			{
				body_part->connect(container_part.get());
			}

			return result;
		}

		unit & spawn(std::string const& name, point2 location)
		{
			unit result;

			// create

			auto sprite = m_sprites->make_shared(name);
			auto transform = m_transforms.make_shared(location);
			auto body = m_bodies.make_shared();
			auto container = m_containers.make_shared();

			// setup

			for (int i = 0; i != 100; ++i)
			{
				auto itm = std::make_shared<rl::item>();
				itm->set_name("item #" + std::to_string(i));
				itm->add({ rl::effect::ore_power, 0x100, 0x500 });
				container->add(itm);
			}

			body->health().create();

			// assemble

			sprite->connect(transform.get());
			transform->connect(body.get());
			body->connect(container.get());

			result.add(sprite);
			result.add(transform);
			result.add(body);
			result.add(container);

			result.enable();

			return result;
		}

	public:
		factory(es::sprite_system * sprites)
			: m_sprites(sprites)
		{
		}

	private:
		es::sprite_system * m_sprites;
		es::transform_system m_transforms;
		es::body_system m_bodies;
		es::container_system m_containers;
	};
}