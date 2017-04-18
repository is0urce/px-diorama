// name: unit_builder.hpp
// type: c++ header
// auth: is0urce
// desc: c++ class

#pragma once

#include "factory.hpp"
#include "es/unit.hpp"

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
		std::shared_ptr<unit> assemble()
		{
			auto result = std::make_shared<unit>();

			link_components();
			compose_unit(*result);

			return result;
		}

	public:
		unit_builder(factory * builder)
			: m_factory(builder)
		{
			if (!builder) throw std::runtime_error("px::unit_builder::ctor() - factory is null");
		}

	private:
		void link_components()
		{
			if (m_sprite && m_transform)	m_sprite->connect(m_transform.get());
			if (m_transform && m_body)		m_transform->connect(m_body.get());
			if (m_body && m_container)		m_body->connect(m_container.get());
		}
		template <typename Container>
		void compose_unit(Container & product)
		{
			if (m_transform)	product.add(m_transform);
			if (m_sprite)		product.add(m_sprite);
			if (m_body)			product.add(m_body);
			if (m_container)	product.add(m_container);
		}

	private:
		factory * m_factory;

		shared_ptr<transform_component> m_transform;
		shared_ptr<sprite_component> m_sprite;
		shared_ptr<body_component> m_body;
		shared_ptr<container_component> m_container;
	};
}