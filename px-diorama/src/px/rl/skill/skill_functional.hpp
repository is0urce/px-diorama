// name: skill_functional.hpp
// type: c++ header
// desc: class
// auth: is0urce

#pragma once

#include "skill_impact.hpp"

#include <functional>

namespace px {
	namespace rl {

		template <typename User, typename Target, typename Area>
		class skill_functional
			: public skill_impact<User, Target, Area>
		{
		public:
			typedef std::function<void(user_type, target_type)> target_fn;
			typedef std::function<bool(user_type, target_type)> target_predicate;
			typedef std::function<void(user_type, area_type)> area_fn;
			typedef std::function<bool(user_type, area_type)> area_predicate;

		public:
			virtual ~skill_functional()
			{
			}
			skill_functional()
				: m_targeted(false)
			{
			}
			skill_functional(bool targeted, target_fn tf, target_predicate tfc, area_fn gf, area_predicate gfc)
				: m_targeted(targeted)
				, m_target(tf)
				, m_target_check(tfc)
				, m_ground(gf)
				, m_ground_check(gfc)
			{
			}
			skill_functional(target_fn tf, target_predicate tfc)
				: m_targeted(true)
				, m_target(tf)
				, m_target_check(tfc)
			{
				if (!tf) throw std::logic_error("px::skill::skill(...) - tf is null");
			}
			skill_functional(area_fn gf, area_predicate gfc)
				: m_targeted(false)
				, m_ground(gf)
				, m_ground_check(gfc)
			{
				if (!gf) throw std::logic_error("px::skill::skill(...) - gf is null");
			}

		protected:

			// override

			virtual bool targeted_skill() const override
			{
				return m_targeted;
			}
			virtual void use_skill(user_type user, target_type target) override
			{
				if (!m_target) throw std::logic_error("px::targeted_ability::use_abitity(..) - m_target function is null");

				m_target(user, target);
			}
			virtual void use_skill(user_type user, area_type area) override
			{
				if (!m_ground) throw std::logic_error("px::targeted_ability::use_abitity(..) - m_ground function is null");

				m_ground(user, area);
			}
			virtual bool useable_skill(user_type user, target_type target) const override
			{
				return m_target_check && m_target_check(user, target);
			}
			virtual bool useable_skill(user_type user, area_type area) const override
			{
				return m_ground_check && m_ground_check(user, area);
			}

			// aux functions

		public:
			static skill_functional create_area(area_fn gf, area_predicate gfc)
			{
				return skill_functional(gf, gfc);
			}
			static skill_functional create_target(target_fn tf, target_predicate tfc)
			{
				return skill_functional(tf, tfc);
			}

		private:
			target_fn			m_target;
			target_predicate	m_target_check;
			area_fn				m_ground;
			area_predicate		m_ground_check;

			bool				m_targeted;
		};
	}
}