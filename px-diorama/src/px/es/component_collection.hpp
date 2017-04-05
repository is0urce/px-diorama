// name: component_collection.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include "component.hpp"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace px {
	namespace es {

		template <typename Pointer>
		class component_collection
		{
		public:
			typedef Pointer component_ptr;

		public:
			void add(component_ptr component)
			{
				if (!component) throw std::runtime_error("px::es::component_collection::add(component) - component is null");

				m_components.push_back(component);
			}
			// remove specified component (O=n)
			void remove(component_ptr component)
			{
				m_components.erase(std::remove(std::begin(m_components), std::end(m_components), component));
			}
			// remove component by type (O=n)
			template<typename C>
			void remove()
			{
				m_components.erase(std::remove_if(std::begin(m_components), std::end(m_components), [&](auto const& c) { return dynamic_cast<C*>(c.get()); }));
			}
			// remove all components
			void clear()
			{
				m_components.clear();
			}

			template <typename Operator>
			void enumerate(Operator && fn)
			{
				for (auto & component : m_components)
				{
					std::forward<Operator>(fn)(component);
				}
			}
			template <typename Operator>
			void enumerate(Operator && fn) const
			{
				for (auto const& component : m_components)
				{
					std::forward<Operator>(fn)(component);
				}
			}

			// activate all components in a collection
			void enable()
			{
				for (auto & component : m_components)
				{
					component->activate();
				}
			}

			// deactivate all components in a collection
			void disable()
			{
				for (auto & component : m_components)
				{
					component->deactivate();
				}
			}

			// querry component by type
			template<typename Sub>
			auto component() const -> decltype(dynamic_pointer_cast<Sub>(component_ptr{}))
			{
				decltype(dynamic_pointer_cast<Sub>(component_ptr{})) result;

				for (auto & c : m_components)
				{
					result = dynamic_pointer_cast<Sub>(c);
					if (result) break;
				}
				return result;
			}

		private:
			std::vector<component_ptr> m_components;
		};
	}
}