// name: transform_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include "transform.hpp"
#include <px/es/component.hpp>
#include <px/es/link.hpp>

namespace px {
	class transform_component final
		: public transform
		, public es::component
		, link_dispatcher<transform_component>
	{
	public:
		void incarnate(space_type * space)
		{
			if (m_space != space)
			{
				if (active())
				{
					if (m_space)
					{
						m_space->remove(x(), y(), this);
					}
					if (space)
					{
						space->add(x(), y(), this);
					}
				}
				m_space = space;
			}
		}
		void move(point2 const& direction)
		{
			place(m_current + direction);
		}
		void place(point2 destination)
		{
			if (m_space && active())
			{
				m_space->move(x(), y(), this, destination.x(), destination.y());
			}
			m_current = destination;
		}

	public:
		virtual ~transform_component()
		{
		}

	protected:
		virtual void activate_component() override
		{
			if (m_space)
			{
				m_space->add(x(), y(), this);
			}
		}
		virtual void deactivate_component() override
		{
			if (m_space)
			{
				m_space->remove(x(), y(), this);
			}
		}
	};
}