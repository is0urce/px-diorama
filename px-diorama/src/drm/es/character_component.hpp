// name: character_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include <px/rl/skill/skill_functional.hpp>
#include <px/rl/skill/skill_instance.hpp>
#include <px/rl/skill/skill_state.hpp>
#include <px/rl/skill/skill_book.hpp>
#include <px/rl/skill/skill_set.hpp>

#include <px/rl/skill/skill.hpp>

namespace px {

	class body_component;

	class character_component final
		: public es::component
		, public es::link_dispatcher<character_component>
	{
	public:
		typedef body_component * actor_type;
		typedef point2 const& area_type;

		typedef rl::skill<actor_type, actor_type, area_type> instance_type;
		typedef rl::skill_book<std::string, rl::skill_functional<actor_type, actor_type, area_type>, instance_type::state_type> book_type;

	public:
		virtual ~character_component()
		{
		}
		character_component()
		{
			m_skills.provide(&m_book);
		}

	private:
		book_type m_book;
		rl::skill_set<book_type, instance_type> m_skills;
	};

}