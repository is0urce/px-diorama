// name: environment-io.cpp
// type: c++

#include "environment.hpp"

#include "serialization.hpp"

#include "es/unit.hpp"
#include "es/unit_builder.hpp"
#include "es/unit_component.hpp"

#include <px/rl/equipment_slot_ext.hpp>
#include <px/rl/effect_ext.hpp>

#include <algorithm>
#include <fstream>
#include <memory>

namespace px {
	namespace {

		// setup item properties from document
		template <typename Document>
		void init_item(Document const& document, rl::item & i)
		{
			auto name = document.find("name");
			if (name != document.end()) {
				i.set_name(*name);
			}

			auto enhancements_node = document.at("enhancements");
			for (auto const& enhancement_node : enhancements_node) {
				auto type_node = enhancement_node.at("effect");
				auto v0_node = enhancement_node.find("value0");
				auto v1_node = enhancement_node.find("value1");

				uint32_t v0 = 0;
				uint32_t v1 = 0;

				if (v0_node != enhancement_node.end()) v0 = *v0_node;
				if (v1_node != enhancement_node.end()) v1 = *v1_node;

				rl::effect efx = rl::to_effect(type_node);

				i.add(rl::enhancement<rl::effect>(efx, 0, v0, v1));
			}
		}

		// returns bounds of a cell
		rectangle cell_bounds(point2 const& cell)
		{
			return rectangle(cell * point2(cell_width, cell_height), point2(cell_width, cell_height));
		}

		// returns path to binary blueprints directory
		std::string depot_blueprint(std::string const& blueprint_tag)
		{
			return std::string(blueprint_directory) + blueprint_tag + std::string(blueprint_extension);
		}

		// returns path to json schemes directory
		std::string depot_scheme(std::string const& scheme_tag)
		{
			return std::string(scheme_directory) + scheme_tag + std::string(scheme_extension);
		}

		// returns true if mobile in cell bound and have required persistency
		// not const, so it uses faster proxy querying of type
		template <unit_persistency Persistency, typename Pointer>
		bool scene_predicate(Pointer & mobile, rectangle const& bounds) {

			px_assert(mobile);

			if (mobile->persistency() == Persistency) {
				transform_component * transform = mobile->transform();
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
				else if (auto deposit = dynamic_cast<deposit_component const*>(part.get())) {
					archive(unit_component::deposit);
				}
				else if (auto player = dynamic_cast<player_component const*>(part.get())) {
					archive(unit_component::player);
				}
				else if (auto npc = dynamic_cast<npc_component const*>(part.get())) {
					archive(unit_component::npc);
					archive(*npc);
				}
				else {
					archive(unit_component::undefined);
				}
			});
		}
		template <typename Archive>
		inline void load_unit(unit_builder & builder, Archive & archive)
		{
			size_t size;			// total components in unit
			unit_component variant;	// current component type

			archive(size);
			for (size_t i = 0; i != size; ++i) {
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
				case unit_component::deposit: {
					builder.add_deposit();
					break;
				}
				case unit_component::player: {
					builder.add_player();
					break;
				}
				case unit_component::npc: {
					archive(*builder.add_npc());
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

		template <typename Operator>
		void load_units(std::string const& path, factory & factory, Operator && execution)
		{
			auto input = input_stream(path);
			SAVE_INPUT_ARCHIVE archive(input);

			// units
			size_t size;
			archive(size);

			for (size_t i = 0; i != size; ++i) {

				unit_builder builder(factory);
				load_unit(builder, archive);

				execution(builder);
			}
		}

		template <typename Container, typename Predicate>
		void save_units(std::string const& path, Container & container, Predicate && predicate)
		{
			auto output = output_stream(path);
			SAVE_OUTPUT_ARCHIVE archive(output);

			// serialize

			size_t size = std::count_if(container.begin(), container.end(), predicate);
			archive(size);

			for (auto & unit : container) {
				if (predicate(unit)) {
					save_unit(*unit, archive);
				}
			}
		}
	}


	void environment::save()
	{
		save(quicksave_repository);
	}
	void environment::load()
	{
		load(quicksave_repository);
	}

	void environment::save(std::string const& save_name)
	{
		// finish ongoing transactions
		m_ui.close_transactions();

		// save
		save_meta();
		save_main();

		// dump on disc
		repository destination(save_name);
		m_repository.save(destination);
	}
	void environment::load(std::string const& save_name)
	{
		// end current game
		end();

		// copy to this repository
		repository source(save_name);
		m_repository.load(source);

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
		save_units(m_repository.depot_main(), m_units, [](auto const& /* mobile_ptr */) { return true; });
	}
	void environment::load_main()
	{
		transform_component * player = nullptr;

		load_units(m_repository.depot_main(), *m_factory, [&](auto & builder) {

			auto mobile = builder.assemble();
			spawn(mobile);

			// set as a player, if this componen was used 
			if (builder.has_player()) {
				player = mobile->transform();
			}
		});

		impersonate(player);
	}

	void environment::archive_scene(point2 const& cell)
	{
		m_ui.close_transactions(); // finish ongoing transactions (possible with subjects in scene)

		rectangle scene_bounds = cell_bounds(cell);
		auto serialized_predicate = [scene_bounds](auto & mobile_ptr) {	return scene_predicate<unit_persistency::serialized>(mobile_ptr, scene_bounds);	};
		auto temporary_predicate = [scene_bounds](auto & mobile_ptr) { return scene_predicate<unit_persistency::temporary>(mobile_ptr, scene_bounds); };

		save_units(m_repository.depot_scene(cell), m_units, serialized_predicate); // serialization

		m_units.erase(std::remove_if(m_units.begin(), m_units.end(), serialized_predicate), m_units.end());	// remove archived objects from array
		m_units.erase(std::remove_if(m_units.begin(), m_units.end(), temporary_predicate), m_units.end());	// also throw away temporaries
	}
	void environment::restore_scene(point2 const& cell)
	{
		std::string path = m_repository.depot_scene(cell);
		if (m_repository.has_scene(path)) {
			load_units(path, *m_factory, [&](auto & builder) {
				spawn(builder.assemble());
			});
		}
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
	environment::unit_ptr environment::compile_unit(std::string const& scheme_name)
	{
		auto document = depot::load_document(depot_scheme(scheme_name));
		unit_builder builder(*m_factory);

		// transform position

		auto transform_node = document.find("transform");
		if (transform_node != document.end()) {
			point2 position;
			position.set<0>(transform_node->at("x"));
			position.set<1>(transform_node->at("y"));
			builder.add_transform(position);
		}

		// sprite appearance

		auto sprite_node = document.find("sprite");
		if (sprite_node != document.end()) {
			std::string url = sprite_node->at("url");
			builder.add_sprite(url);
		}

		// body

		auto body_node = document.find("body");
		if (body_node != document.end()) {
			auto body = builder.add_body();

			// entity

			auto tag_node = body_node->find("tag");
			if (tag_node != body_node->end()) {
				body->set_tag(*tag_node);
			}

			auto name_node = body_node->find("name");
			if (name_node != body_node->end()) {
				body->set_name(*name_node);
			}

			auto description_node = body_node->find("description");
			if (description_node != body_node->end()) {
				body->set_description(description_node.value());
			}

			// mass

			auto transperency_node = body_node->find("transparent");
			if (transperency_node != body_node->end()) {
				body->mass().make_transparent(*transperency_node);
			}

			auto traversable_node = body_node->find("traversable");
			if (traversable_node != body_node->end()) {
				for (unsigned int layer : *traversable_node) {
					body->mass().make_traversable(static_cast<rl::traverse>(layer));
				}
			}

			auto traverse_node = body_node->find("traverse");
			if (traverse_node != body_node->end()) {
				for (unsigned int layer : *traverse_node) {
					body->traverse().make_traversable(static_cast<rl::traverse>(layer));
				}
			}

			// faction

			auto faction_node = body_node->find("faction");
			if (faction_node != body_node->end()) {
				body->join_faction(faction_node.value());
			}

			// resources

			auto hp_node = body_node->find("hp");
			if (hp_node != body_node->end()) {
				body->health().create(hp_node.value());
			}

			auto mp_node = body_node->find("mp");
			if (mp_node != body_node->end()) {
				body->energy().create(mp_node.value());
			}

			// equipment
			auto equipment_node = body_node->find("equipment");
			if (equipment_node != body_node->end()) {
				for (auto const& slot_node : *equipment_node) {
					auto type_node = slot_node.at("slot");
					auto item_node = slot_node.at("item");

					//rl::equipment_slot slot = ;
					rl::item i;

					init_item(item_node, i);

					body->equip(rl::to_equipment_slot(type_node), i);
				}
			}

		}

		// character

		auto character_node = document.find("character");
		if (character_node != document.end()) {
			auto character = builder.add_character();

			auto skills_node = character_node->find("skills");
			if (skills_node != character_node->end()) {
				for (auto const& skill_node : *skills_node) {
					character->learn_skill(skill_node);
				}
			}
		}

		// npc component

		auto npc_node = document.find("ai");
		if (npc_node != document.end()) {
			auto npc = builder.add_npc();
		}

		// build and return

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