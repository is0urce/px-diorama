// name: faction_relation.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

// reputation system

#pragma once

namespace px
{
	namespace core
	{
		class faction_relation
		{
		public:
			int reputation(unsigned int subject_faction, unsigned int target_faction) const
			{
				return subject_faction == target_faction ? 100 : -100;
			}
		};
	}
}