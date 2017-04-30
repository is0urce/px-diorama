// name: transform_system.hpp
// type: c++ header
// auth: is0urce
// desc: class

#pragma once

#include <px/es/basic_system.hpp>

#include "transform_component.hpp"

namespace px {
	namespace es {

		class transform_system final
			: protected basic_system<transform_component>
		{
		public:
			auto make_shared(point2 position)
			{
				auto result = basic_system::make_shared();
				setup(*result, position);
				return result;
			}
			auto make_unique(point2 position)
			{
				auto result = basic_system::make_unique();
				setup(*result, position);
				return result;
			}
			auto make_std(point2 position)
			{
				auto result = basic_system::make_std();
				setup(*result, position);
				return result;
			}
			transform_component::space_type * space() noexcept
			{
				return &m_space;
			}

		public:
			virtual ~transform_system()
			{
				basic_system::clear(); // remove items before space release
			}
			transform_system()
				: m_space(64)
			{
			}

		private:
			void setup(transform_component & element, point2 position)
			{
				element.place(position);
				element.store_position();
				element.incarnate(&m_space);
			}

		private:
			transform_component::space_type m_space;
		};
	}
}