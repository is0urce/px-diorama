// name: unit_builder.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

#pragma once

#include "factory.hpp"
#include "unit.hpp"

#include <memory>
#include <string>
#include <stdexcept>

namespace px {

	class unit_builder final
	{
	public:
		auto add_transform(point2 location)
		{
			return m_transform = m_factory->make_transform(location);
		}
		auto add_sprite(std::string name)
		{
			return m_sprite = m_factory->make_sprite(name);
		}
		auto add_body()
		{
			return m_body = m_factory->make_body();
		}
		auto add_container()
		{
			return m_container = m_factory->make_container();
		}
		auto add_storage()
		{
			return m_storage = m_factory->make_storage();
		}
		auto add_player()
		{
			return m_player = m_factory->make_player();
		}
		auto add_character()
		{
			return m_character = m_factory->make_character();
		}
		auto add_npc()
		{
			return m_npc = m_factory->make_npc();
		}

		// combine
		std::shared_ptr<unit> assemble()
		{
			auto result = std::make_shared<unit>();

			link_components();
			compose_unit(*result);

			return result;
		}

		// fetch part status
		bool is_player() const
		{
			return m_player;
		}

		// fetch current transform part
		auto current_transform()
		{
			return m_transform;
		}

	public:
		unit_builder(factory & builder)
			: m_factory(&builder)
		{
		}
		unit_builder(factory * builder)
			: m_factory(builder)
		{
			if (!builder) throw std::runtime_error("px::unit_builder::ctor() - factory is null");
		}
		unit_builder(unit_builder const&) = delete;
		unit_builder& operator=(unit_builder const&) = delete;

	private:
		void link_components()
		{
			if (m_transform) {
				if (m_body)			m_transform->connect(m_body.get());
			}

			if (m_sprite) {
				if (m_transform)	m_sprite->connect(m_transform.get());
			}

			if (m_body) {
				if (m_transform)	m_body->connect(m_transform.get());
				if (m_container)	m_body->connect(m_container.get());
				if (m_character)	m_body->connect(m_character.get());
				if (m_storage)		m_body->assign_useable(m_storage.get()); // assign extra polymorphic links
			}

			if (m_storage && m_container)	m_storage->connect(m_container.get());
			if (m_container && m_transform) m_container->connect(m_transform.get());

			if (m_npc) {
				if (m_transform)	m_npc->connect(m_transform.get());
				if (m_character)	m_npc->connect(m_character.get());
			}
		}
		template <typename Container>
		void compose_unit(Container & product)
		{
			if (m_transform)	product.add(m_transform);
			if (m_sprite)		product.add(m_sprite);
			if (m_body)			product.add(m_body);
			if (m_container)	product.add(m_container);
			if (m_storage)		product.add(m_storage);
			if (m_player)		product.add(m_player);
			if (m_character)	product.add(m_character);
			if (m_npc)			product.add(m_npc);
		}

	private:
		factory *							m_factory;

		shared_ptr<transform_component>		m_transform;
		shared_ptr<sprite_component>		m_sprite;
		shared_ptr<body_component>			m_body;
		shared_ptr<character_component>		m_character;
		shared_ptr<container_component>		m_container;
		shared_ptr<storage_component>		m_storage;
		shared_ptr<player_component>		m_player;
		shared_ptr<npc_component>			m_npc;
	};
}