// name: environment-io.cpp
// type: c++

#include "environment.hpp"

#include "serialization.hpp"

#include "es/unit.hpp"
#include "es/unit_builder.hpp"
#include "es/unit_component.hpp"

#include <fstream>

namespace px {

	namespace {
		char const* const quicksave_path = "quicksave.sav";
		char const* const blueprints_path = "data/blueprints/";
		char const* const blueprint_extension = ".dat";

		std::string depot_filename(std::string const& blueprint_tag)
		{
			return std::string(blueprints_path) + blueprint_tag + std::string(blueprint_extension);
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
		save(quicksave_path);
	}
	void environment::load()
	{
		load(quicksave_path);
	}
	void environment::save(std::string const& name)
	{
		// finish current activities
		m_ui.close_transactions();

		// make archives
		std::ofstream output(name, SAVE_OUTPUT_MODE);
		SAVE_OUTPUT_ARCHIVE archive(output);

		// save game state
		archive(m_turn);

		// units
		size_t size = m_units.size();
		archive(size);
		for (auto const& unit : m_units) {
			save_unit(*unit, archive);
		}
	}
	void environment::load(std::string const& name)
	{
		// finish current activities
		end();

		// make archives
		std::ifstream input(name, SAVE_INPUT_MODE);
		if (!input.is_open()) throw std::runtime_error("px::environment::load() - file error, name=" + name);
		SAVE_INPUT_ARCHIVE archive(input);

		// load game state
		archive(m_turn);

		// units
		size_t total_units;
		archive(total_units);
		for (size_t i = 0; i != total_units; ++i) {
			unit_builder builder(*m_factory);
			load_unit(builder, archive);
			auto mobile = builder.assemble();

			spawn(mobile);

			// set as a player, if this componen was used 
			if (builder.is_player()) impersonate(mobile->transform());
		}
	}

	void environment::export_unit(unit const& mobile, std::string const& blueprint_name) const
	{
		// make archives
		std::ofstream output(depot_filename(blueprint_name), SAVE_OUTPUT_MODE);
		SAVE_OUTPUT_ARCHIVE archive(output);

		save_unit(mobile, archive);
	}
	environment::unit_ptr environment::import_unit(std::string const& blueprint_name)
	{
		std::string name = depot_filename(blueprint_name);

		std::ifstream input(name, SAVE_INPUT_MODE);
		if (!input.is_open()) throw std::runtime_error("px::environment::import_unit() - file error, name=" + name);
		SAVE_INPUT_ARCHIVE archive(input);

		unit_builder builder(*m_factory);
		load_unit(builder, archive);
		return builder.assemble();
	}
	environment::unit_ptr environment::import_unit(std::string const& blueprint_name, point2 location)
	{
		std::string name = depot_filename(blueprint_name);

		std::ifstream input(name, SAVE_INPUT_MODE);
		if (!input.is_open()) throw std::runtime_error("px::environment::import_unit() - file error, name=" + name);
		SAVE_INPUT_ARCHIVE archive(input);

		unit_builder builder(*m_factory);
		load_unit(builder, archive);

		builder.current_transform()->move(location);

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