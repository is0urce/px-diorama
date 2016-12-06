// name: unit.hpp
// type: c++ header
// desc: class declaration
// auth: is0urce

#include <px/es/component_collection.hpp>

#include "transform_component.hpp"

namespace px
{
	class unit
		: public es::component_collection
	{
	public:
		transform_component * transform()
		{
			if (!m_transform)
			{
				m_transform = component<transform_component>().get();
			}
			return m_transform;
		}
		transform_component * transform() const
		{
			if (m_transform)
			{
				return m_transform;
			}
			return component<transform_component>().get();
		}
	private:
		transform_component * m_transform;
	};
}