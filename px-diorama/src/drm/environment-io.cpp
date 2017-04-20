#include "environment.hpp"

#include <cereal/archives/binary.hpp>
//#include <cereal/archives/xml.hpp>
#include <fstream>

namespace px {

	namespace {
		const char * quicksave_path = "quick.sav";
	}

	void environment::save()
	{
		save(quicksave_path);
	}
	void environment::load()
	{
		load(quicksave_path);
	}
	void environment::save(std::string const& name)
	{
		std::ofstream output(name);

		//cereal::XMLOutputArchive archive(output);
		cereal::BinaryOutputArchive archive(output);

		save_units(archive);
	}
	void environment::load(std::string const& name)
	{
		std::ifstream input(name);

		//cereal::XMLOutputArchive archive(output);
		cereal::BinaryInputArchive archive(input);

		m_units.clear();
		load_units(archive);
	}
}