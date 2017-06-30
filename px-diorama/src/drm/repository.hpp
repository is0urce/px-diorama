// name: save.hpp
// type: c++
// auth: is0urce
// desc: load documents

#pragma once

#include "settings.hpp"

#include <px/common/point.hpp>
#include <px/common/string_ext.hpp>

#include <filesystem>
#include <stdexcept>
#include <string>

namespace fs = std::experimental::filesystem;

namespace px {

	class repository final
	{
	public:
		void remove()
		{
			fs::remove_all(m_directory);
		}
		void create()
		{
			fs::create_directory(m_directory);
		}
		void reset()
		{
			remove();
			create();
		}
		bool exists() const
		{
			return fs::exists(m_directory);
		}

		void save(repository & destination) const
		{
			copy(*this, destination);
		}
		void load(repository const& destination)
		{
			if (!destination.exists()) throw std::runtime_error("repository not exists");

			copy(destination, *this);
		}

		// data

		std::string depot_scene(point2 const& cell) const
		{
			return depot_name("scene_" + to_string(cell));
		}
		std::string depot_main() const
		{
			return depot_name("scene_main");
		}
		std::string depot_meta() const
		{
			return depot_name("meta");
		}

		bool has_scene(std::string const& depot_name) const
		{
			return exists(depot_name);
		}
		bool has_main()
		{
			return exists(depot_main());
		}
		bool has_meta() const
		{
			return exists(depot_meta());
		}

		bool clear_scene(std::string const& name)
		{
			return clear(name);
		}
		bool clear_main()
		{
			return clear(depot_main());
		}
		bool clear_meta()
		{
			return clear(depot_meta());
		}

	public:
		repository(std::string const& name)
			: m_directory(save_directory / fs::path(name))
		{
		}

	private:
		std::string depot_name(std::string const& scene_name) const
		{
			fs::path path = scene_name;
			path.replace_extension(save_extension);
			return (m_directory / path).string();
		}
		bool exists(std::string const& name) const
		{
			return fs::exists(name);
		}
		bool clear(std::string const& name) const
		{
			return fs::remove(name);
		}
		static void move(repository & source, repository & destination)
		{
			destination.reset();

			for (auto & entry : fs::directory_iterator(source.m_directory)) {
				fs::path path = entry.path();
				fs::rename(path, destination.m_directory / path.filename());
			}
		}
		static void copy(repository const& source, repository & destination)
		{
			destination.reset();

			for (auto & entry : fs::directory_iterator(source.m_directory)) {
				fs::path path = entry.path();
				fs::copy_file(path, destination.m_directory / path.filename(), fs::copy_options::overwrite_existing);
			}
		}

	private:
		fs::path m_directory;
	};
}