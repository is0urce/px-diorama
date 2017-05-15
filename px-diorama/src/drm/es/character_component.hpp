// name: character_component.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include <px/es/component.hpp>
#include <px/es/link_dispatcher.hpp>

#include <px/rl/skill/skill.hpp>
#include <px/rl/skill/skill_book.hpp>
#include <px/rl/skill/skill_functional.hpp>
#include <px/rl/skill/skill_set.hpp>

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
		typedef rl::skill_set<instance_type, book_type> skillset_type;

	public:
		instance_type * get_skill(size_t slot)
		{
			return m_skills.get_skill(slot);
		}
		void learn_skill(std::string tag)
		{
			m_skills.learn_skill(tag);
		}
		void provide_book(book_type * skillbook)
		{
			m_skills.provide_book(skillbook);
		}

	public:
		virtual ~character_component()
		{
			m_skills.clear_skills();
		}
		character_component()
		{
		}

	private:
		rl::skill_set<instance_type, book_type> m_skills;
	};

}