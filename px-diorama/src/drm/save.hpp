// name: save.hpp
// type: c++
// auth: is0urce
// desc: load documents

#pragma once

#include "settings.hpp"

#include <px/common/point.hpp>

#include <filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace px {

	class save final
	{
	public:
		std::string depot_scene(point2 const& cell)
		{
			fs::path path = "scene_" + std::to_string(cell.x()) + "_" + std::to_string(cell.y());
			path.replace_extension(save_extension);
			return (m_directory / path).string();
		}
		std::string depot_meta()
		{
			return m_directory.string();
		}

		void directory()
		{

		}

	public:
		save(std::string const& name)
			: m_directory(save_directory / fs::path(name))
		{
			fs::remove(m_directory);
			fs::create_directory(m_directory);
		}

	private:
		fs::path m_directory;
	};
}