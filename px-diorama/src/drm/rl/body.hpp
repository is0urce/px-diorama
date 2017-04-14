// name: body.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <px/rl/resource.hpp>
#include <px/rl/entity.hpp>
#include <px/rl/soc/reputation_subject.hpp>

#include "drm/rl/inventory.hpp"

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

			void clear()
			{
				m_inventory.clear();
				m_hp.remove();
				m_mp.remove();
				clear_faction();
			}

		private:
			resource m_hp;
			resource m_mp;
			inventory m_inventory;
		};
	}
}