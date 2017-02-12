// name: reputation_subject.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

namespace px
{
	namespace rl
	{
		class reputation_subject
		{
		public:
			// reputation

			unsigned int faction() const
			{
				return m_faction;
			}
			void join_faction(unsigned int index)
			{
				m_faction = index;
			}
			void clear_faction()
			{
				m_faction = 0;
			}

			int reputation(reputation_subject const& with)
			{
				return m_faction == with.m_faction ? 100 : -100;
			}
			bool hostile(reputation_subject const& with)
			{
				return reputation(with) < 0;
			}
			bool ally(reputation_subject const& with)
			{
				return reputation(with) > 0;
			}

		public:
			reputation_subject(unsigned int faction_index)
				: m_faction(faction_index)
			{
			}
			reputation_subject()
				: reputation_subject(0)
			{
			}

		private:
			unsigned int m_faction;
		};
	}
}