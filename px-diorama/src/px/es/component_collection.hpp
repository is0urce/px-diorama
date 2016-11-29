// name: component_collection.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include "component.hpp"

#include <algorithm>
#include <memory>
#include <vector>

namespace px
{
	namespace es
	{
		class component_collection
		{
		public:
			typedef std::shared_ptr<component> component_ptr;

		public:
			void add(component_ptr c)
			{
				m_components.push_back(c);
			}
			// remove specified component (O=n)
			void remove(component_ptr c)
			{
				m_components.erase(std::remove(std::begin(m_components), std::end(m_components), c));
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

			void activate()
			{
				for (auto & component : m_components)
				{
					component->activate();
				}
			}
			void deactivate()
			{
				for (auto & component : m_components)
				{
					component->deactivate();
				}
			}

			// querry component by type
			template<typename T>
			std::shared_ptr<T> component() const
			{
				std::shared_ptr<T> cast;
				for (auto & c : m_components)
				{
					cast = std::dynamic_pointer_cast<T>(c);
					if (cast) break;
				}
				return cast;
			}

		private:
			std::vector<component_ptr> m_components;
		};
	}
}