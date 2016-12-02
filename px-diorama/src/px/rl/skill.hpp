// name: skill.hpp
// type: c++ header
// desc: class
// auth: is0urce

#pragma once

#include <px/rl/i_ability.hpp>
#include <px/rl/skill_base.hpp>

#include <functional>

namespace px
{
	namespace rl
	{
		template <typename User, typename Target>
		class skill : public i_ability<User, Target>, public skill_base
		{
		public:
			typedef User user_type;
			typedef Target target_type;
			typedef std::function<void(user_type, target_type)> target_fn;
			typedef std::function<bool(user_type, target_type)> target_check_fn;
			typedef std::function<void(user_type, const point2&)> ground_fn;
			typedef std::function<bool(user_type, const point2&)> ground_check_fn;

		private:
			bool m_targeted;
			target_fn m_target;
			target_check_fn m_target_check;
			ground_fn m_ground;
			ground_check_fn m_ground_check;

		public:
			skill(target_fn tf, target_check_fn tfc) :
				m_targeted(true),
				m_target(tf), m_target_check(tfc)
			{
				if (!tf) throw std::logic_error("px::targeted_ability::targeted_ability() - tf is null");
			}
			skill(target_fn tf) :
				m_targeted(true),
				m_target(tf)
			{
				if (!tf) throw std::logic_error("px::targeted_ability::targeted_ability() - tf is null");
			}
			skill(ground_fn gf, ground_check_fn gfc) :
				m_targeted(false),
				m_ground(gf), m_ground_check(gfc)
			{
				if (!gf) throw std::logic_error("px::targeted_ability::targeted_ability() - gf is null");
			}
			skill(ground_fn gf) :
				m_targeted(false),
				m_ground(gf)
			{
				if (!gf) throw std::logic_error("px::targeted_ability::targeted_ability() - gf is null");
			}
			virtual ~skill() {}

		public:
			static skill create_ground(ground_fn gf, ground_check_fn gfc)
			{
				return skill(gf, gfc);
			}
			static skill create_target(target_fn tf, target_check_fn tfc)
			{
				return skill(tf, tfc);
			}

		protected:
			virtual bool targeted_ability() const override
			{
				return m_targeted;
			}
			virtual void use_ability(user_type u, target_type t) override
			{
				if (!m_target) throw std::logic_error("px::targeted_ability::use_abitity(..) - m_target function is null");

				m_target(u, t);
			}
			virtual void use_ability(user_type u, point2 const& t) override
			{
				if (!m_ground) throw std::logic_error("px::targeted_ability::use_abitity(..) - m_ground function is null");

				m_ground(u, t);
			}
			virtual bool useable_ability(user_type u, target_type t) const override
			{
				return !(on_cooldown() || (m_target_check && !m_target_check(u, t)));
			}
			virtual bool useable_ability(user_type u, point2 const& t) const override
			{
				return !(on_cooldown() || (m_ground_check && !m_ground_check(u, t)));
			}
		};
	}
}