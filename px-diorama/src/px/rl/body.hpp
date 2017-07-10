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
#include <px/rl/mass.hpp>
#include <px/rl/traverse.hpp>
#include <px/rl/traverse_options.hpp>

#include <cstdint>
#include <memory>
#include <string>

namespace px {
	namespace rl {

		class body
			: public entity
			, public reputation_subject
			, public equipment<rl::equipment_slot, rl::item>
		{
		public:
			typedef resource<int32_t> resource_type;
			typedef rl::mass<rl::traverse> mass_type;
			typedef rl::traverse_options<rl::traverse> traverse_type;

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
			
			bool dead() const noexcept
			{
				return m_hp && m_hp->empty();
			}
			bool alive() const noexcept
			{
				return m_hp && !m_hp->empty();
			}

			mass_type const& mass() const noexcept
			{
				return m_mass;
			}
			mass_type & mass() noexcept
			{
				return m_mass;
			}
			traverse_type const& traverse() const noexcept
			{
				return m_traverse;
			}
			traverse_type & traverse() noexcept
			{
				return m_traverse;
			}
			std::string blood() const noexcept
			{
				return m_blood;
			}
			void set_blood(std::string blood_tag)
			{
				m_blood = blood_tag;
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
				archive(m_mass);
				archive(m_traverse);
				archive(m_blood);
			}

		private:
			resource_type	m_hp;
			resource_type	m_mp;
			mass_type		m_mass;
			traverse_type	m_traverse;
			std::string		m_blood;
		};
	}
}