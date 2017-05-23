// name: body.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/resource.hpp>
#include <px/rl/entity.hpp>
#include <px/rl/reputation_subject.hpp>

#include <memory>

namespace px {
	namespace rl {

		class body
			: public entity
			, public reputation_subject
		{
		public:
			typedef resource<int> resource;

		public:

			// resources
			const resource& health() const noexcept
			{
				return m_hp;
			}
			resource& health() noexcept
			{
				return m_hp;
			}
			const resource& energy() const noexcept
			{
				return m_mp;
			}
			resource& energy() noexcept
			{
				return m_mp;
			}

			void clear_body()
			{
				m_hp.remove();
				m_mp.remove();
				clear_faction();
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				entity::serialize(archive);
				reputation_subject::serialize(archive);
				archive(m_hp, m_mp);
			}

		private:
			resource m_hp;
			resource m_mp;
		};
	}
}