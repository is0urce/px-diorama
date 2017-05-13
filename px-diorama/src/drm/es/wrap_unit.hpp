// name: wrap_unit.cpp

// script bindings for game object unit

#pragma once

namespace px {

	class wrap_unit
	{
	public:
		void place(point2 location)
		{
			auto pawn = transform();
			if (pawn) {
				pawn->place(location);
			}
		}

	public:
		wrap_unit(body_component * body)
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