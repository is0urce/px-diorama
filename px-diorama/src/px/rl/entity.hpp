// name: entity.hpp
// type: c++ header
// desc: class definition
// auth: is0urce

#pragma once

// something with generic characteristics as name, tag, description

namespace px
{
	namespace rl
	{
		class entity
		{
		private:
			std::string m_name;
			std::string m_description;
			std::string m_tag;

		public:
			std::string name() const
			{
				return m_name;
			}
			void set_name(std::string name)
			{
				m_name = name;
			}
			std::string tag() const
			{
				return m_tag;
			}
			void set_tag(std::string resource_name)
			{
				m_tag = resource_name;
			}
			std::string description() const
			{
				return m_description;
			}
			void set_description(std::string text)
			{
				m_description = text;
			}
		};
	}
}