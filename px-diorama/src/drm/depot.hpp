// name: depot.hpp
// type: c++
// auth: is0urce
// desc: load documents

#pragma once

#include <json.hpp>

#include <fstream>
#include <stdexcept>
#include <string>

namespace px {

	class depot
	{
	public:
		static auto load_document(std::string const& document_depot)
		{
			std::ifstream file(document_depot);
			if (!file.is_open()) throw std::runtime_error("error opening file path=" + document_depot);
			return nlohmann::json::parse(file);
		}
	};
}