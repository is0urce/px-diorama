// name: script_unit.cpp

// script bindings for game object unit

#pragma once

#include "body_component.hpp"
#include "transform_component.hpp"

namespace px {

	class script_unit
	{
	public:

		// pawn

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

		// resources

		bool dead() const
		{
			return m_body && m_body->health() && m_body->health()->empty();
		}
		bool valid() const
		{
			return m_body != nullptr;
		}
		void damage(int dmg)
		{
			if (m_body)	{
				auto & health = m_body->health();
				if (health) health->damage(dmg);
			}
		}
		void heal(int hp)
		{
			if (m_body) {
				auto & health = m_body->health();
				if (health) health->restore(hp);
			}
		}
		void drain(int mana)
		{
			if (m_body) {
				auto & energy = m_body->energy();
				if (energy) energy->restore(mana);
			}
		}
		void innervate(int mana)
		{
			if (m_body) {
				auto & energy = m_body->energy();
				if (energy) energy->restore(mana);
			}
		}

		// social

		int reputation(script_unit const& b) const
		{
			return m_body && b.m_body ? m_body->reputation(*b.m_body) : 0;
		}

		// special
		transform_component * transform()
		{
			if (!m_transform) {
				m_transform = m_body ? m_body->linked<transform_component>() : nullptr;
			}
			return m_transform;
		}

		// provide equality operator
		bool operator==(script_unit & b)
		{
			return m_body == b.m_body;
		}

	public:
		script_unit(body_component * unit_body)
			: m_body(unit_body)
			, m_transform(nullptr)
		{
		}

	private:
		body_component *		m_body;
		transform_component *	m_transform;
	};

}