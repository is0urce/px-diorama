// name: character_system.hpp

#pragma once

#include "character_component.hpp"

#include "script/script.hpp"

#include <px/common/assert.hpp>
#include <px/es/basic_system.hpp>

namespace px {
	namespace es {

		class character_system final
			: public basic_system<character_component, 100000>
		{
		public:
			typedef character_component::book_type book_type;

		public:
			auto make_shared()
			{
				auto result = basic_system::make_shared();
				setup_component(*result);
				return result;
			}
			auto make_unique()
			{
				auto result = basic_system::make_unique();
				setup_component(*result);
				return result;
			}
			auto make_std()
			{
				auto result = basic_system::make_std();
				setup_component(*result);
				return result;
			}

			void provide_environment(environment * bind)
			{
				m_script.provide_environment(bind);
			}
			void load_skill(std::string const& path)
			{
				m_script.load_skill(path, m_book);
			}

		public:
			character_system()
			{
				load_skills();
			}

		private:
			void setup_component(character_component & element)
			{
				element.provide_book(&m_book);
			}
			void load_skills()
			{
				load_skill("data/scripts/sk_v_teleport.lua");
				load_skill("data/scripts/sk_v_melee.lua");

				load_skill("data/scripts/sk_s_smite.lua");
				load_skill("data/scripts/sk_s_rend.lua");
				load_skill("data/scripts/sk_s_flurry.lua");
				load_skill("data/scripts/sk_i_pain.lua");

				load_skill("data/scripts/sk_o_export.lua");
				load_skill("data/scripts/sk_o_import.lua");
			}

		private:
			script									m_script;
			book_type								m_book;
		};
	}
}