// name: skill.hpp
// type: c++ header
// auth: is0urce
// desc: class declaration

#pragma once

#include "skill_instance.hpp"
#include "skill_state.hpp"

namespace px {
	namespace rl {

		template <typename User, typename Target, typename Area>
		class skill
			: public skill_instance<User, Target, Area>
		{
		public:
			skill_state & state() noexcept
			{
				return m_state;
			}
			skill_state const& state() const noexcept
			{
				return m_state;
			}

		protected:
			virtual bool useable_instance() const override
			{
				return !m_state.on_cooldown();
			}
			virtual void use_instance() override
			{
				m_state.start_cooldown();
			}

		private:
			skill_state m_state;
		};

	}
}