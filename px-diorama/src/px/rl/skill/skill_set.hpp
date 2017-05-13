// name: skill_set.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include "skill_book.hpp"

#include <vector>
#include <memory>

namespace px
{
	namespace rl
	{
		template <typename Instance, typename Book>
		class skill_set final
		{
		public:
			typedef Book book_type;
			typedef Instance instance_type;
			typedef typename book_type::tag_type tag_type;

		public:
			void provide_book(book_type * book)
			{
				m_book = book;
			}
			void invalidate_skills()
			{
				//if (!m_provider) throw std::runtime_error("px::rl::skill_set::invalidate() - book provider is null");

				//for (auto & slot : m_skills)
				//{
				//	auto s = m_provider->find(it->tag);

				//	if (s == m_provider->end()) throw std::runtime_error("px::rl::skill_set::invalidate - book has no skill with name " + it->tag);

				//	slot.assign(s->second);
				//}
			}
			void learn_skill(tag_type tag)
			{
				if (!m_book) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book provider is null");
	
				if (auto * record = m_book->fetch(tag)) {

					m_skills.emplace_back(std::get<0>(*record), &std::get<1>(*record));
				}
			}
			void replace_skill(tag_type tag, size_t slot)
			{
				if (!m_book) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book provider is null");

				if (auto * record = m_book->fetch(tag)) {

					m_skills[slot] = { std::get<0>(*record), &std::get<1>(*record) };
				}
			}
			void clear_skills()
			{
				m_skills.clear();
			}
			size_t skils_total() const
			{
				return m_skills.size();
			}

			instance_type & at(size_t slot)
			{
				return m_skills.at(slot);
			}
			template <size_t Slot>
			instance_type & at()
			{
				return m_skills.at(slot);
			}
			void cooldown(unsigned int time)
			{
				//for (auto & skill : m_skills) {
				//	if (skill.state().cooldown(time)) {
				//		skill.ability->cooldown(time);
				//	}
				//}
			}

		public:
			skill_set()
				: m_book(nullptr)
			{
			}
			skill_set(book_type * book)
				: m_book(book)
			{
			}
			skill_set(skill_set const&) = delete;
			skill_set & operator=(skill_set const&) = delete;

		private:
			std::vector<instance_type>	m_skills;	// array of learned/selected skills
			book_type *					m_book;	// create and invalidate source
		};
	}
}