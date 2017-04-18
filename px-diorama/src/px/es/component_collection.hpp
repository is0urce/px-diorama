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
				std::for_each(std::begin(m_components), std::end(m_components), std::forward<Operator>(fn));
			}
			template <typename Operator>
			void enumerate(Operator && fn) const
			{
				std::for_each(std::begin(m_components), std::end(m_components), std::forward<Operator>(fn));
			}

			// activate all components in a collection
			void enable()
			{
				enumerate([](auto & component) { component->activate(); });
			}

			// deactivate all components in a collection
			void disable()
			{
				enumerate([](auto & component) { component->deactivate(); });
			}

			// querry component by type
			// return shared_ptr<U>
			template<typename U>
			auto component() const -> decltype(dynamic_pointer_cast<U>(component_ptr{}))
			{
				decltype(dynamic_pointer_cast<U>(component_ptr{})) result;

				for (auto & c : m_components)
				{
					result = dynamic_pointer_cast<U>(c);
					if (result) break;
				}
				return result;
			}
			void swap(component_collection & that) noexcept
			{
				std::swap(m_components, that.m_components);
			}

		public:
			component_collection()
			{
			}
			component_collection(component_collection && that) noexcept
				: unit()
			{
				swap(that);
			}
			component_collection& operator=(component_collection && that) noexcept
			{
				swap(that);
				return *this;
			}
			component_collection(component_collection const&) = delete;
			component_collection& operator=(component_collection const&) = delete;

		private:
			std::vector<component_ptr> m_components;
		};
	}
}