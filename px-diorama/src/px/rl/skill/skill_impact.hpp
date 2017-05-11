// name: skill_impact.hpp
// type: c++ header
// desc: interface template class declaration
// auth: is0urce

// abstract ability used on target or on specified position

#pragma once

namespace px {
	namespace rl {

		template <typename User, typename Target, typename Area>
		class skill_impact
		{
		public:
			typedef User user_type;
			typedef Target target_type;
			typedef Area area_type;

		public:
			bool targeted() const
			{
				return targeted_ability();
			}

			bool useable(user_type user, target_type target) const
			{
				return useable_ability(user, target);
			}
			bool useable(user_type user, area_type area) const
			{
				return useable_ability(user, area);
			}

			void use(user_type user, target_type t)
			{
				use_ability(user, t);
			}
			void use(user_type user, area_type t)
			{
				use_ability(user, t);
			}

			bool try_use(user_type user, target_type target)
			{
				if (useable_ability(user, target)) {
					use_ability(user, target);
					return true;
				}
				return false;
			}

			bool try_use(user_type user, area_type area)
			{
				if (useable_ability(user, area)) {
					use_ability(user, area);
					return true;
				}
				return false;
			}

		public:
			virtual ~skill_impact()
			{
			}

		protected:
			virtual void use_skill(user_type user, target_type target) = 0;
			virtual void use_skill(user_type user, area_type area) = 0;
			virtual bool useable_skill(user_type user, target_type target) const = 0;
			virtual bool useable_skill(user_type user, area_type area) const = 0;
			virtual bool targeted_skill() const = 0;
		};
	}
}