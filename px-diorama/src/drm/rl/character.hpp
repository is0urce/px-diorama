// name: character.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/common/point.hpp>

#include <px/rl/skill/skill.hpp>
#include <px/rl/skill/skill_book.hpp>
#include <px/rl/skill/skill_functional.hpp>
#include <px/rl/skill/skill_set.hpp>

#include <string>

namespace px {

	class body_component;

	namespace rl {

		class character
		{
		public:
			typedef body_component * actor_type;
			typedef point2 const& area_type;

			typedef skill<actor_type, actor_type, area_type> instance_type;
			typedef typename instance_type::state_type state_type;
			typedef skill_book<std::string, skill_functional<actor_type, actor_type, area_type>, state_type> book_type;
			typedef skill_set<instance_type, book_type> skillset_type;

		public:
			instance_type * get_skill(size_t slot)
			{
				return m_panel.get_skill(slot);
			}
			instance_type const* get_skill(size_t slot) const
			{
				return m_panel.get_skill(slot);
			}
			size_t count_skills() const
			{
				return m_panel.size();
			}
			void learn_skill(std::string tag)
			{
				m_panel.learn_skill(tag);
			}
			void provide_book(book_type * skillbook)
			{
				m_panel.provide_book(skillbook);
			}
			void clear_skills()
			{
				m_panel.clear_skills();
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_panel);
			}

		private:
			skill_set<instance_type, book_type> m_panel;
		};
	}
}