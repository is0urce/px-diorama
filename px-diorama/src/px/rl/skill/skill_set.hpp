// name: skill_set.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/common/assert.hpp>

#include "skill_book.hpp"

#include <vector>
#include <memory>

namespace px
{
	namespace rl
	{
		template <typename Instance, typename Book>
		class skill_set
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
			void learn_skill(tag_type tag)
			{
				if (!m_book) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book provider is null");
	
				auto * record = m_book->fetch(tag);
				px_assert(record);
				if (record) {
					m_skills.emplace_back(std::get<0>(*record), &std::get<1>(*record));
				}
			}
			void replace_skill(tag_type tag, size_t slot)
			{
				if (!m_book) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book provider is null");

				auto * record = m_book->fetch(tag);
				px_assert(record);
				if (record) {
					m_skills[slot] = { std::get<0>(*record), &std::get<1>(*record) };
				}
			}
			void invalidate_skills()
			{
				for (size_t i = 0, size = m_skills.size(); i != size; ++i) {
					replace_skill(m_skills[i].state().tag(), i);
				}
			}
			void clear_skills()
			{
				m_skills.clear();
			}
			size_t size() const
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
			instance_type * get_skill(size_t index)
			{
				return index < m_skills.size() ? &m_skills[index] : nullptr;
			}
			void cooldown_by(unsigned int delta)
			{
				for (auto & skill : m_skills) {
					skill.state().cooldown_by(time);
				}
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_skills);
				invalidate_skills();
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
			book_type *					m_book;		// create and invalidate source
		};
	}
}