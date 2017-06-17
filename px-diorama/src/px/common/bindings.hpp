// name: bindings.hpp
// type: c++ header
// desc: class to translate system virtual key messages to shell virtual key messages
// auth: is0urce

#pragma once

#include <stdexcept>
#include <tuple>
#include <unordered_map>

namespace px {

	template <typename SK, typename VK>
	class bindings final
	{
	public:
		typedef SK sys_key;
		typedef VK v_key;

	public:
		bindings()
		{
		}
		template <typename Document>
		bindings(Document && document)
		{
			load(std::forward<Document>(document));
		}

	public:
		template <typename Document>
		static bindings from_document(Document && document)
		{
			bindings result;
			result.load(std::forward<Document>(document));
			return result;
		}

	public:
		template <typename Document>
		void load(Document && document)
		{
			try
			{
				for (auto const& binding : document) {
					long long action_index = binding["action"];
					for (long long key : binding["keys"]) {
						bind(static_cast<SK>(key), static_cast<VK>(action_index));
					}
				}
			}
			catch (std::exception & exc)
			{
				throw std::runtime_error("error while loading bindings, what=" + std::string(exc.what()));
			}
		}
		void bind(SK key, VK vkey)
		{
			m_bindings[key] = vkey;
		}
		bool find(SK key, VK & vkey) const
		{
			auto i = m_bindings.find(key);
			if (i != m_bindings.end()) {
				vkey = i->second;
				return true;
			}
			return false;
		}
		std::tuple<VK, bool> find(SK key) const
		{
			auto it = m_bindings.find(key);
			if (it = m_bindings.end()) return std::tuple<VK, bool>(VK{}, false);
			return std::tuple<VK, bool>(it->second, true);
		}
		VK select(SK key, VK or_else) const
		{
			find(key, or_else);
			return or_else;
		}
		VK operator[](SK key) const noexcept
		{
			return m_bindings[key];
		}
		VK at(SK key) const
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

	private:
		std::unordered_map<SK, VK> m_bindings;
	};
}