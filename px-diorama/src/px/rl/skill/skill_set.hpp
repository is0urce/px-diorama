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
		template <typename Book, typename Instance>
		class skill_set
		{
		public:
			typedef Book book_type;
			typedef Instance instance_type;
			typedef typename book_type::tag_type tag_type;

		public:
			void provide(book_type const* book)
			{
				m_provider = book;
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
			auto add_skill(tag_type tag, size_t slot)
			{
				//if (!m_provider) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book provider is null");

				//auto it = m_provider->find(tag);

				//if (it == m_provider->end()) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book has no skill with name " + tag);

				//m_skills.reserve(slot + 1);
				//m_skills.emplace(m_skills.begin() + slot, tag, it->second);

				//return skill(slot);
			}
			auto add_skill(tag_type tag)
			{
				return add_skill(tag, m_skills.size());
			}
			void clear_skills()
			{
				m_skills.clear();
			}
			size_t skils_total() const
			{
				return m_skills.size();
			}

			instance_type * skill(size_t slot)
			{
				return (slot < m_skills.size()) ? m_skills[slot].ability.get() : nullptr;
			}
			template <size_t Slot>
			instance_type * skill()
			{
				return (Slot < m_skills.size()) ? m_skills[Slot].ability.get() : nullptr;
			}
			void cooldown(unsigned int time)
			{
				//for (auto & skill : m_skills)
				//{
				//	if (skill.ability)
				//	{
				//		skill.ability->cooldown(time);
				//	}
				//}
			}

		public:
			skill_set()
				: m_provider(nullptr)
			{
			}
			skill_set(book_type const* book)
				: m_provider(book)
			{
			}
			skill_set(skill_set const&) = delete;
			skill_set & operator=(skill_set const&) = delete;

		private:
			std::vector<instance_type>	m_skills;	// array of learned/selected skills
			book_type const*			m_provider;	// create and invalidate source
		};
	}
}