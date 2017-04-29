// name: unit.hpp
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include <px/es/component_collection.hpp>
#include <px/common/shared_ptr.hpp>

//#include "transform_component.hpp"

#include "unit_persistency.hpp"

namespace px {

	class transform_component;

	class unit
		: public es::component_collection<shared_ptr<es::component>>
	{
	public:
		transform_component * transform()
		{
			if (!m_transform) {
				m_transform = component<transform_component>().get();
			}
			return m_transform;
		}
		transform_component const* transform() const
		{
			return m_transform ? m_transform : component<transform_component>().get();
		}
		unit_persistency persistency() const noexcept
		{
			return m_persistency;
		}

	public:
		unit()
			: m_persistency(unit_persistency::serialized)
			, m_transform(nullptr)
		{
		}
		unit(unit const&) = delete;
		unit& operator=(unit const&) = delete;

	private:
		unit_persistency		m_persistency;
		transform_component *	m_transform;
	};
}