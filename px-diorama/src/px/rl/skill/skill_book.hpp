// name: skill_book.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

#include <map>
#include <tuple>

namespace px {
	namespace rl {

		template <typename Tag, typename Impact, typename State = void*>
		class skill_book final
		{
		public:
			typedef Tag tag_type;
			typedef Impact impact_type;
			typedef State state_type;
			typedef std::tuple<state_type, impact_type> record_type;

		public:
			template <typename ...Args>
			void emplace(Tag name, state_type state, Args &&... args)
			{
				m_map.emplace(name, record_type{ state, impact_type{std::forward<Args>(args)...} });
			}
			//record_type const& operator[](Tag name)
			//{
			//	return m_map[name];
			//}

		public:
			skill_book()
			{
			}
			skill_book(skill_book const&) = delete;
			skill_book & operator=(skill_book const&) = delete;

		private:
			std::map<tag_type, record_type> m_map;
			std::map<tag_type, impact_type> m_impact;
		};
	}
}