// name: script_unit.cpp

// script bindings for game object unit

#pragma once

#include "body_component.hpp"
#include "transform_component.hpp"

namespace px {

	class script_unit
	{
	public:
		void place(point2 location)
		{
			auto * pawn = transform();
			if (pawn) {
				pawn->place(location);
			}
		}
		point2 position()
		{
			auto * pawn = transform();
			return pawn ? pawn->position() : point2(0, 0);
		}

	public:
		script_unit(body_component * body)
			: m_body(body)
			, m_transform(nullptr)
		{
		}

	private:
		transform_component * transform()
		{
			if (!m_transform) {
				m_transform = m_body ? m_body->linked<transform_component>() : nullptr;
			}
			return m_transform;
		}

	private:
		body_component *		m_body;
		transform_component *	m_transform;
	};

}