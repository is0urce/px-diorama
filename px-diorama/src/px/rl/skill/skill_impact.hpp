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
				return targeted_skill();
			}

			bool useable(user_type user, target_type target) const
			{
				return useable_skill(user, target);
			}
			bool useable(user_type user, area_type area) const
			{
				return useable_skill(user, area);
			}

			void use(user_type user, target_type target)
			{
				use_skill(user, target);
			}
			void use(user_type user, area_type area)
			{
				use_skill(user, area);
			}

			bool try_use(user_type user, target_type target)
			{
				if (useable_skill(user, target)) {
					use_skill(user, target);
					return true;
				}
				return false;
			}

			bool try_use(user_type user, area_type area)
			{
				if (useable_skill(user, area)) {
					use_skill(user, area);
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