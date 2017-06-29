// name: save.hpp
// type: c++
// auth: is0urce
// desc: load documents

#pragma once

#include "settings.hpp"

#include <px/common/point.hpp>
#include <px/common/string_ext.hpp>

#include <filesystem>
#include <string>

namespace fs = std::experimental::filesystem;

namespace px {

	class repository final
	{
	public:
		std::string depot_scene(point2 const& cell) const
		{
			fs::path path = "scene_" + to_string(cell);
			path.replace_extension(save_extension);
			return depot_scene("scene" + to_string(cell));
		}
		std::string depot_scene() const
		{
			return depot_scene("scene_main");
		}
		std::string depot_meta() const
		{
			return m_directory.string();
		}
		void clear_all()
		{
			fs::remove_all(m_directory);
		}
		void create()
		{
			fs::create_directory(m_directory);
		}
		void reset()
		{
			clear_all();
			create();
		}
		bool has_scene(std::string const& depot_name) const
		{
			return fs::exists(depot_name);
		}
		bool has_meta() const
		{
			return fs::exists(depot_meta());
		}
		bool clear_scene(std::string const& depot_name)
		{
			return fs::remove(depot_name);
		}
		bool clear_meta()
		{
			return fs::remove(depot_meta());
		}

	public:
		repository(std::string const& name)
			: m_directory(save_directory / fs::path(name))
		{
			create();
		}

	private:
		std::string depot_scene(std::string const& scene_name) const
		{
			fs::path path = scene_name;
			path.replace_extension(save_extension);
			return (m_directory / path).string();
		}

	private:
		fs::path m_directory;
	};
}