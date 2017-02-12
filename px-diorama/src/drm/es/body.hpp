// name: body.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/bar.hpp>
#include <px/rl/entity.hpp>
#include <px/rl/soc/reputation_subject.hpp>

#include <memory>

namespace px
{
	namespace rl
	{
		class body
			: public entity
			, public reputation_subject
		{
		public:
			typedef bar<int> bar;
			typedef std::unique_ptr<bar> bar_ptr;

		public:

			// resources

			void set_health(bar::value_type amount)
			{
				m_hp = std::make_unique<bar>(amount);
			}
			void set_energy(bar::value_type amount)
			{
				m_mp = std::make_unique<bar>(amount);
			}
			void remove_health()
			{
				m_hp.release();
			}
			void remove_energy()
			{
				m_mp.release();
			}
			const bar* health() const
			{
				return m_hp.get();
			}
			bar* health()
			{
				return m_hp.get();
			}
			const bar* energy() const
			{
				return m_mp.get();
			}
			bar* energy()
			{
				return m_mp.get();
			}

		private:
			bar_ptr m_hp;
			bar_ptr m_mp;
		};
	}
}