// name: body.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/equipment.hpp>
#include <px/rl/entity.hpp>
#include <px/rl/equipment_slot.hpp>
#include <px/rl/loot/item.hpp>
#include <px/rl/resource.hpp>
#include <px/rl/reputation_subject.hpp>

#include <cstdint>
#include <memory>

namespace px {
	namespace rl {

		class body
			: public entity
			, public reputation_subject
			, public equipment<rl::equipment_slot, rl::item>
		{
		public:
			typedef resource<int32_t> resource_type;

		public:

			// resources
			resource_type const& health() const noexcept
			{
				return m_hp;
			}
			resource_type & health() noexcept
			{
				return m_hp;
			}
			resource_type const& energy() const noexcept
			{
				return m_mp;
			}
			resource_type & energy() noexcept
			{
				return m_mp;
			}

			void clear_body()
			{
				m_hp.remove();
				m_mp.remove();
				clear_faction();
				strip();
			}

			template <typename Archive>
			void serialize(Archive & archive)
			{
				archive(static_cast<entity &>(*this));
				archive(static_cast<reputation_subject &>(*this));
				archive(m_hp, m_mp);
				archive(static_cast<equipment &>(*this));
			}

		private:
			resource_type m_hp;
			resource_type m_mp;
		};
	}
}