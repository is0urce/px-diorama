// name: bindings.hpp
// type: c++ header
// desc: class to translate system virtual key messages to shell virtual key messages
// auth: is0urce

#pragma once

#include <unordered_map>

namespace px
{
	template <typename SK, typename VK>
	class bindings final
	{
	public:
		typedef SK sys_key;
		typedef VK v_key;

	private:
		std::unordered_map<SK, VK> m_bindings;

	public:
		bindings() {}

	public:
		void bind(SK key, v_key vkey)
		{
			m_bindings[key] = vkey;
		}
		bool find(SK key, VK& vkey) const
		{
			auto i = m_bindings.find(key);
			if (i != m_bindings.end())
			{
				vkey = i->second;
				return true;
			}
			return false;
		}
		VK select(SK key, VK or_else) const
		{
			find(key, or_else);
			return or_else;
		}
		VK operator[](SK key) const
		{
			return m_bindings.at(key);
		}


		void bind(SK key1, SK key2, v_key vkey)
		{
			bind(key1, vkey);
			bind(key2, vkey);
		}
		void bind(SK key1, SK key2, SK key3, v_key vkey)
		{
			bind(key1, vkey);
			bind(key2, vkey);
			bind(key3, vkey);
		}
	};
}