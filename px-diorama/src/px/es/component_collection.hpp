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
				for (auto & component : m_components)
				{
					cast = std::dynamic_pointer_cast<T>(component);
					if (cast) break;
				}
				if (!cast) throw std::runtime_error("px::es::component_collection::component<T>() - we're not serving subclasses here");
				return cast;
			}

			// remove component by type
			template<typename C>
			bool remove()
			{
				for (auto it = m_components.begin(), last = m_components.end(); it != last; ++it)
				{
					auto cast = std::dynamic_pointer_cast<C>(*it);
					if (cast)
					{
						m_components.erase(it);
						return true;
					}
				}
				return false;
			}

		private:
			std::vector<component_ptr> m_components;
		};
	}
}