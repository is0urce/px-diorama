// name: skill_book.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include "skill.hpp"

#include <map>

namespace px
{
	namespace rl
	{
		template <typename Tag, typename User, typename Target>
		class skill_book : public std::map<Tag, rl::skill<User, Target>>
		{
		public:
			typedef Tag tag_type;
			typedef rl::skill<User, Target> skill_type;
			typedef typename skill_type::target_fn tf;
			typedef typename skill_type::target_check_fn tfc;
			typedef typename skill_type::ground_fn gf;
			typedef typename skill_type::ground_check_fn gfc;

		public:
			skill_type & add_target(tag_type name, tf action_fn, tfc condition_fn)
			{
				// std::pair<map::<key, value>::iterator,bool> - True for Insertion, False for No Insertion.
				auto ib_pair = emplace(name, skill_type{ action_fn, condition_fn });

				if (!ib_pair.second) throw std::runtime_error("px::rl::skill_book::add_target(..) - skill with same tag exists");

				ib_pair.first->second.set_tag(name);
				return ib_pair.first->second; // value of an iterator
			}
			skill_type & add_ground(tag_type name, gf action_fn, gfc condition_fn)
			{
				auto pair = emplace(name, skill_type{ action_fn, condition_fn });

				if (!pair.second) throw std::runtime_error("px::rl::skill_book::add_target(..) - skill with same tag exists");

				pair.first->second.set_tag(name);
				return pair.first->second; // value of an iterator
			}
		};
	}
}