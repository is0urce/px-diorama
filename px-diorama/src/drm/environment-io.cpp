// name: environment-io.cpp
// type: c++

#include "environment.hpp"

#include "serialization.hpp"

#include "es/unit.hpp"
#include "es/unit_builder.hpp"
#include "es/unit_component.hpp"

#include <algorithm>
#include <fstream>
#include <memory>

namespace px {

	namespace {

		// returns bounds of a cell
		rectangle cell_bounds(point2 const& cell)
		{
			return rectangle(cell * point2(cell_width, cell_height), point2(cell_width, cell_height));
		}

		std::string depot_blueprint(std::string const& blueprint_tag)
		{
			return std::string(blueprint_directory) + blueprint_tag + std::string(blueprint_extension);
		}

		template <unit_persistency Persistency, typename Pointer>
		bool scene_predicate(Pointer & mobile_ptr, rectangle const& bounds) {

			px_assert(mobile_ptr);

			if (mobile_ptr->persistency() == Persistency) {
				transform_component * transform = mobile_ptr->transform();
				px_assert(transform);
				return transform && bounds.contains(transform->position());
			}

			return false;
		}

		std::ifstream input_stream(std::string const& name) {
			std::ifstream input(name, SAVE_INPUT_MODE);
			if (!input.is_open()) throw std::runtime_error("px::environment-io::input_stream() - file error, name=" + name);
			return input;
		}
		std::ofstream output_stream(std::string const& name) {
			std::ofstream output(name, SAVE_OUTPUT_MODE);
			if (!output.is_open()) throw std::runtime_error("px::environment-io::output_stream() - file error, name=" + name);
			return output;
		}

		template <typename Archive>
		inline void save_unit(unit const& mobile, Archive & archive)
		{
			size_t total_components = mobile.components_size();
			archive(total_components);

			mobile.enumerate_components([&](auto const& part) {
				if (auto transform = dynamic_cast<transform_component const*>(part.get())) {
					archive(unit_component::transform);
					archive(*transform);
				}
				else if (auto sprite = dynamic_cast<sprite_component const*>(part.get())) {
					archive(unit_component::sprite);
					archive(std::string(sprite->name));
				}
				else if (auto body = dynamic_cast<body_component const*>(part.get())) {
					archive(unit_component::body);
					archive(*body);
				}
				else if (auto container = dynamic_cast<container_component const*>(part.get())) {
					archive(unit_component::container);
					archive(*container);
				}
				else if (auto character = dynamic_cast<character_component const*>(part.get())) {
					archive(unit_component::character);
					archive(*character);
				}
				else if (auto storage = dynamic_cast<storage_component const*>(part.get())) {
					archive(unit_component::storage);
				}
				else if (auto player = dynamic_cast<player_component const*>(part.get())) {
					archive(unit_component::player);
				}
				else {
					archive(unit_component::undefined);
				}
			});
		}
		template <typename Archive>
		inline void load_unit(unit_builder & builder, Archive & archive)
		{
			size_t total_components;
			unit_component variant;

			archive(total_components);
			for (size_t i = 0; i != total_components; ++i) {
				archive(variant);
				switch (variant) {
				case unit_component::transform: {
					archive(*builder.add_transform({})); // transform disabled by default, so we can write into internal state
					break;
				}
				case unit_component::sprite: {
					std::string tag;
					archive(tag);
					builder.add_sprite(tag);
					break;
				}
				case unit_component::body: {
					archive(*builder.add_body());
					break;
				}
				case unit_component::container: {
					archive(*builder.add_container());
					break;
				}
				case unit_component::character: {
					archive(*builder.add_character());
					break;
				}
				case unit_component::storage: {
					builder.add_storage();
					break;
				}
				case unit_component::player: {
					builder.add_player();
					break;
				}
				case unit_component::undefined: {
					break; // there was unserilized (temporary) component, just skip it
				}
				default:
					// component defined, but not supported (version conflict?)
					throw std::runtime_error("px::environment::load_unit(builder, archive) - unknown component");
				}
			}
		}
	}

	void environment::save()
	{
		save(quicksave_name);
	}
	void environment::load()
	{
		load(quicksave_name);
	}

	void environment::save(std::string const& save_name)
	{
		// finish current activities
		m_ui.close_transactions();

		// save
		save_meta();
		save_main();

		// dump on disc
		repository quicksave(save_name);
		m_repository.save(quicksave);
	}
	void environment::load(std::string const& save_name)
	{
		// finish current activities
		end();

		// copy to this repository
		repository quicksave(save_name);
		m_repository.load(quicksave);

		// load
		load_meta();
		load_main();
	}

	void environment::save_meta()
	{
		// make archives
		auto output = output_stream(m_repository.depot_meta());
		SAVE_OUTPUT_ARCHIVE archive(output);

		// save game state
		archive(m_turn);
	}
	void environment::load_meta()
	{
		// make archives
		auto input = input_stream(m_repository.depot_meta());
		SAVE_INPUT_ARCHIVE archive(input);

		// load game state
		archive(m_turn);
	}

	void environment::save_main()
	{
		// make archives
		auto output = output_stream(m_repository.depot_main());
		SAVE_OUTPUT_ARCHIVE archive(output);

		// units
		size_t size = m_units.size();
		archive(size);
		for (auto const& unit : m_units) {
			save_unit(*unit, archive);
		}
	}
	void environment::load_main()
	{
		// make archives
		auto input = input_stream(m_repository.depot_main());
		SAVE_INPUT_ARCHIVE archive(input);

		// units
		size_t size;
		archive(size);
		for (size_t i = 0; i != size; ++i) {
			unit_builder builder(*m_factory);
			load_unit(builder, archive);

			auto mobile = builder.assemble();

			spawn(mobile);

			// set as a player, if this componen was used 
			if (builder.has_player()) {
				impersonate(mobile->transform());
			}
		}
	}

	void environment::archive_scene(point2 const& cell)
	{
		std::string	scene_path = m_repository.depot_scene(cell);
		px_assert(!m_repository.has_scene(scene_path));

		auto output = output_stream(scene_path);
		SAVE_OUTPUT_ARCHIVE archive(output);

		// finish current activities

		m_ui.close_transactions();

		// lambdas

		rectangle scene_bounds = cell_bounds(cell);
		auto serialized_predicate = [scene_bounds](auto & mobile_ptr) {
			return scene_predicate<unit_persistency::serialized>(mobile_ptr, scene_bounds);
		};
		auto temporary_predicate = [scene_bounds](auto & mobile_ptr) {
			return scene_predicate<unit_persistency::temporary>(mobile_ptr, scene_bounds);
		};

		// serialize

		size_t size = std::count_if(m_units.begin(), m_units.end(), serialized_predicate);
		archive(size);

		for (auto & unit : m_units) {
			if (serialized_predicate(unit)) {
				save_unit(*unit, archive);
			}
		}

		// remove archived objects from array, also throw away temporaries

		m_units.erase(std::remove_if(m_units.begin(), m_units.end(), serialized_predicate), m_units.end());
		m_units.erase(std::remove_if(m_units.begin(), m_units.end(), temporary_predicate), m_units.end());
	}
	void environment::restore_scene(point2 const& cell)
	{
		std::string scene_name = m_repository.depot_scene(cell);
		px_assert(m_repository.has_scene(scene_name));

		auto input = input_stream(scene_name);
		SAVE_INPUT_ARCHIVE archive(input);

		// units
		size_t size;
		archive(size);
		for (size_t i = 0; i != size; ++i) {

			unit_builder builder(*m_factory);
			load_unit(builder, archive);

			spawn(builder.assemble());
		}

		m_repository.clear_scene(scene_name);
	}

	// UNIT EXPORT-IMPORT

	void environment::export_unit(unit const& mobile, std::string const& blueprint_name) const
	{
		// make archives
		auto output = output_stream(depot_blueprint(blueprint_name));
		SAVE_OUTPUT_ARCHIVE archive(output);

		save_unit(mobile, archive);
	}
	environment::unit_ptr environment::import_unit(std::string const& blueprint_name)
	{
		auto input = input_stream(depot_blueprint(blueprint_name));
		SAVE_INPUT_ARCHIVE archive(input);

		unit_builder builder(*m_factory);
		load_unit(builder, archive);

		return builder.assemble();
	}
	environment::unit_ptr environment::import_unit(std::string const& blueprint_name, point2 location)
	{
		auto input = input_stream(depot_blueprint(blueprint_name));
		SAVE_INPUT_ARCHIVE archive(input);

		unit_builder builder(*m_factory);
		load_unit(builder, archive);

		if (auto transform = builder.transform()) {
			transform->place(location);
			transform->store_position();
		}

		return builder.assemble();
	}
	size_t environment::mass_export(point2 const& position)
	{
		size_t exported = 0;
		for (auto const& unit : m_units) {
			auto * transform = unit->transform();
			auto * body = transform->linked<body_component>();

			if (transform && transform->position() == position) {
				std::string name = body ? body->tag() : std::string("unnamed") + std::to_string(exported);
				export_unit(*unit, name);
				++exported;
			}
		}
		return exported;
	}
}