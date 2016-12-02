// name: i_ability.hpp
// type: c++ header
// desc: interface template class declaration
// auth: is0urce

// abstract ability used on target or on specified position

#pragma once

#include <px/common/point.hpp>

namespace px
{
	namespace rl
	{
		template <typename User, typename Target>
		class i_ability
		{
		public:
			typedef User user_type;
			typedef Target target_type;

		public:
			virtual ~i_ability() = 0 {}

		protected:
			virtual void use_ability(user_type u, target_type t) = 0;
			virtual void use_ability(user_type u, const point2 &t) = 0;
			virtual bool useable_ability(user_type u, target_type t) const = 0;
			virtual bool useable_ability(user_type u, const point2 &t) const = 0;
			virtual bool targeted_ability() const = 0;

		public:
			bool targeted() const { return targeted_ability(); }
			bool useable(user_type u, target_type t) const { return useable_ability(u, t); }
			bool useable(user_type u, const point2 &t) const { return useable_ability(u, t); }
			void use(user_type u, target_type t) { use_ability(u, t); }
			void use(user_type u, const point2 &t) { use_ability(u, t); }

			bool try_use(user_type u, target_type t)
			{
				if (useable_ability(u, t))
				{
					use_ability(u, t);
					return true;
				}
				return false;
			}
			bool try_use(user_type u, const point2 &t)
			{
				if (useable_ability(u, t))
				{
					use_ability(u, t);
					return true;
				}
				return false;
			}
		};
	}
}