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

			unsigned int faction() const noexcept
			{
				return m_faction;
			}
			void join_faction(unsigned int index) noexcept
			{
				m_faction = index;
			}
			void clear_faction() noexcept
			{
				m_faction = 0;
			}

			int reputation(reputation_subject const& with) const noexcept
			{
				return m_faction == with.m_faction ? 100 : -100;
			}
			bool hostile(reputation_subject const& with) const noexcept
			{
				return reputation(with) < 0;
			}
			bool ally(reputation_subject const& with) const noexcept
			{
				return reputation(with) > 0;
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(m_faction);
			}

		public:
			reputation_subject(unsigned int faction_index) noexcept
				: m_faction(faction_index)
			{
			}
			reputation_subject() noexcept
				: reputation_subject(0)
			{
			}

		private:
			unsigned int m_faction;
		};
	}
}