// name: skill_instance.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

// base for instanced abilities with a state
// proxy to base skill functional

#include <px/rl/skill/skill_impact.hpp>

namespace px {
	namespace rl {

		template <typename User, typename Target, typename Area>
		class skill_instance
			: public skill_impact<User, Target, Area>
		{
		public:
			typedef skill_impact<User, Target, Area> impact_type;

		public:
			void link_impact(impact_type * base_impact)
			{
				m_base = base_impact;
			}

		public:
			virtual ~skill_instance()
			{
			}
			skill_instance()
				: m_base(nullptr)
			{
			}
			skill_instance(impact_type const* base_impact)
				: m_base(base_impact)
			{
			}

		protected:
			virtual bool useable_instance() const
			{
				return true;
			}
			virtual void use_instance()
			{
			}

			// override

			virtual bool targeted_skill() const override
			{
				return m_base && m_base->targeted();
			}
			virtual void use_skill(user_type user, target_type target) override
			{
				m_base->use(user, target);
			}
			virtual void use_skill(user_type user, area_type area) override
			{
				m_base->use(user, area);
			}
			virtual bool useable_skill(user_type user, target_type target) const override
			{
				return useable_instance() && m_base && m_base->useable(user, target);
			}
			virtual bool useable_skill(user_type user, area_type area) const override
			{
				return useable_instance() && m_base && m_base->useable(user, area);
			}

		private:
			impact_type * m_base;
		};

	}
}