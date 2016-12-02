// name: skill_set.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once
#include <px/rl/skill_book.hpp>

#include <vector>
#include <memory>

namespace px
{
	namespace rl
	{
		template <typename Tag, typename User, typename Target>
		class skill_set
		{
		public:
			typedef Tag tag_type;
			typedef skill<User, Target> skill_type;
			typedef skill_book<Tag, User, Target> skillbook_type;

			struct record
			{
				tag_type tag;
				std::unique_ptr<skill_type> ability;
				void assign(skill_type skill)
				{
					*ability = skill;
				}
				record(tag_type t, skill_type skill) : tag(t), ability(std::make_unique<skill_type>(skill))
				{
				}
			};


		public:
			void provide(skillbook_type* book)
			{
				m_provider = book;
			}
			void invalidate_skills()
			{
				if (!m_provider) throw std::runtime_error("px::rl::skill_set::invalidate() - book provider is null");

				for (auto &slot : m_skills)
				{
					auto s = m_provider->find(it->tag);

					if (s == m_provider->end()) throw std::runtime_error("px::rl::skill_set::invalidate - book has no skill with name " + it->tag);

					slot.assign(s->second);
				}
			}
			auto add_skill(tag_type tag, size_t slot)
			{
				if (!m_provider) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book provider is null");

				auto it = m_provider->find(tag);

				if (it == m_provider->end()) throw std::runtime_error("px::rl::skill_set::add(name, slot) - book has no skill with name " + tag);

				m_skills.reserve(slot + 1);
				m_skills.emplace(m_skills.begin() + slot, tag, it->second);

				return skill(slot);
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

			skill_type* skill(size_t slot)
			{
				return (slot < m_skills.size()) ? m_skills[slot].ability.get() : nullptr;
			}
			template <size_t Slot>
			skill_type* skill()
			{
				return (Slot < m_skills.size()) ? m_skills[Slot].ability.get() : nullptr;
			}
			void cooldown(unsigned int time)
			{
				for (auto &skill : m_skills)
				{
					if (skill.ability)
					{
						skill.ability->cooldown(time);
					}
				}
			}

		public:
			skill_set() : m_provider(nullptr)
			{
			}
			skill_set(skillbook_type& book) : m_provider(&book)
			{
			}
			skill_set(skill_set const&) = delete;
			skill_set& operator=(skill_set const&) = delete;

		private:
			std::vector<record> m_skills;
			skillbook_type* m_provider;
		};
	}
}