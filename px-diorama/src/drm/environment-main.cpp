// name: environment.main

#include "environment.hpp"

#include "perception.hpp"

#include "es/unit_builder.hpp"
#include "es/unit_component.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <json.hpp>

#include <fstream>

namespace {
	char const* quicksave_path = "quick.sav";
}

namespace px {

	environment::~environment()
	{
		clear();
		m_map.resize({ 0,0 });
	}

	environment::environment()
		: m_factory(std::make_unique<factory>())
		, m_player(nullptr)
		, m_run(true)
		, m_turn(1)
	{
	}

	bool environment::running() const noexcept
	{
		return m_run;
	}
	void environment::shutdown() noexcept
	{
		m_run = false;
	}
	void environment::clear()
	{
		m_units.clear();
		impersonate(nullptr);
	}
	void environment::impersonate(transform_component * player)
	{
		m_player = player;
		m_factory->sprites()->assign_camera(m_player);
	}
	void environment::update(perception & view, double time) const
	{
		// track timestamp from last turn update
		static unsigned int last_turn = 0;
		static double last_time = 0;
		if (m_turn != last_turn) {
			last_turn = m_turn;
			last_time = time;
		}
		auto span = time - last_time;

		// compose sprite batches
		m_factory->sprites()->update(span);
		view.assign_batches(&m_factory->sprites()->batches());

		// compose user interface
		m_ui.main()->draw(view.canvas());
	}
	void environment::turn()
	{
		std::for_each(std::begin(m_units), std::end(m_units), [](auto & unit) {
			auto transform = unit->transform();
			if (transform) transform->store_position();
		});
		++m_turn;
	}

	void environment::target(point2 relative_world_coordinates)
	{
		m_hover = relative_world_coordinates + (m_player ? m_player->position() : point2(0, 0));

		m_ui.lock_target(m_hover, find_any(m_hover));
	}
	void environment::step(point2 const& direction)
	{
		if (!m_player) return;

		point2 destination = m_player->position() + direction;

		if (!m_map.traversable(destination)) return;

		auto blocking = find_any(destination);

		if (!blocking) {
			turn();
			m_player->place(destination);
		}
	}
	void environment::use(unsigned int /*index*/)
	{
	}
	void environment::activate(unsigned int /*mod*/)
	{
		auto target = find_any(m_hover);
		auto body = target ? target->linked<body_component>() : nullptr;
		auto user = m_player ? m_player->linked<body_component>() : nullptr;

		if (body) {
			body->try_use(user, *this);
		}
	}

	std::shared_ptr<unit> environment::spawn(std::string const& name, point2 location)
	{
		// create
		unit_builder builder(m_factory.get());
		auto transform = builder.add_transform(location);
		auto body = builder.add_body();
		auto sprite = builder.add_sprite(name);
		auto container = builder.add_container();
		auto storage = builder.add_storage();

		// setup
		transform->store_position();
		body->health().create();
		body->set_name(name);
		for (int i = 0; i != 10; ++i) {
			auto itm = std::make_shared<rl::item>();
			itm->set_name("item #" + std::to_string(i));
			itm->add({ rl::effect::ore_power, 0x100, 0x500 });
			container->add(itm);
		}

		// add to scene
		auto result = builder.assemble();
		result->enable();
		m_units.push_back(result);

		return result;
	}
	void environment::start()
	{
		// terrain
		generate_terrain();

		// units

		spawn("m_snake", { 50, 50 });
		spawn("m_h_rabling_harvester", { 50, 51 });
		spawn("m_h_rabling_reaper", { 50, 52 });
		spawn("m_h_rabling_shepard", { 50, 53 });

		spawn("m", { 53, 47 });

		spawn("p_table", { 55, 47 });
		spawn("p_bookshelf", { 54, 47 });
		spawn("p_locker", { 55, 48 });
		spawn("p_box", { 55, 49 });

		// player
		auto player = spawn("@", { 54, 46 });

		player->add(m_factory->make_player());
		impersonate(player->transform());
	}
	void environment::generate_terrain()
	{
		point2 range(100, 100);
		m_map.resize(range);
		rectangle(range).enumerate([this](auto const& point) {
			auto & tile = m_map[point];

			tile.transform.move(point);
			tile.transform.store_position();

			tile.ground = m_factory->sprites()->make_unique("#");
			tile.ground->connect(&tile.transform);
			tile.ground->activate();

			tile.mass.make_wall();
		});

		std::mt19937 rng;
		fn::dig_generator dig(100, 100);
		dig.generate(rng, 5, 12, 1, 15);
		matrix2<unsigned char> map(100, 100);
		dig.rasterize(map);
		map.enumerate([this](auto const& point, auto terrain_variant) {
			auto & tile = m_map[point];

			if (terrain_variant == 0) return;

			tile.ground.reset();

			tile.wall = m_factory->sprites()->make_unique("+");
			tile.wall->connect(&tile.transform);
			tile.wall->activate();

			tile.mass.make_ground();
		});
	}

	void environment::add_spritesheet(std::string const& path, bool reverse_y)
	{
		std::ifstream input(path);
		if (!input.is_open()) throw std::runtime_error("px::environment::add_sprite(path, bool) - file not exists, path=" + path);

		auto document = nlohmann::json::parse(input);
		m_factory->sprites()->add_atlas(document["meta"], reverse_y);
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

		archive(m_turn);
		save_units(archive);
	}
	void environment::load(std::string const& name)
	{
		std::ifstream input(name);

		//cereal::XMLInputArchive archive(input);
		cereal::BinaryInputArchive archive(input);

		clear();

		archive(m_turn);
		load_units(archive);
	}
	template <typename Archive>
	void environment::save_units(Archive & archive) const
	{
		size_t size = m_units.size();
		archive(size);
		for (auto const& unit : m_units) {
			save_unit(*unit, archive);
		}
	}
	template <typename Archive>
	void environment::load_units(Archive & archive)
	{
		size_t total_units;
		archive(total_units);
		for (size_t i = 0; i != total_units; ++i) {
			unit_builder builder(*m_factory);
			load_unit(builder, archive);
			auto mobile = builder.assemble();

			mobile->enable();
			m_units.push_back(mobile);

			// set as a player, if this componen was used 
			if (builder.is_player()) impersonate(mobile->transform());
		}
	}
	template <typename Archive>
	void environment::save_unit(unit const& mobile, Archive & archive) const
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

				size_t strlen = std::strlen(sprite->name);
				archive(strlen);
				archive.saveBinary(sprite->name, strlen);
			}
			else if (auto player = dynamic_cast<player_component const*>(part.get())) {
				archive(unit_component::player);
			}
			else {
				archive(unit_component::undefined);
			}
		});
	}
	template <typename Archive, typename Builder>
	void environment::load_unit(Builder & builder, Archive & archive)
	{
		size_t total_components;
		archive(total_components);

		for (size_t i = 0; i != total_components; ++i)
		{
			unit_component variant;
			archive(variant);

			switch (variant) {
			case unit_component::transform:
			{
				auto transform = builder.add_transform({});
				archive(*transform); // should be disabled by default, so we can write to internal values
			}
			break;
			case unit_component::sprite:
			{
				size_t strlen;
				archive(strlen);

				std::vector<char> name(strlen + 1, 0);
				archive.loadBinary(name.data(), strlen);

				builder.add_sprite(name.data());
			}
			break;
			case unit_component::container:
			case unit_component::storage:
			case unit_component::body:
				break;
			case unit_component::player:
			{
				builder.add_player();
			}
			break;
			case unit_component::undefined:
			{
				// there was unserilized component, just skip it
			}
			break;
			default:
				// component defined, but not supported (version conflict?)
				throw std::runtime_error("px::environment::load_unit(builder, archive) - unknown component");
			}
		}
	}

	ui::panel * environment::ui() noexcept
	{
		return m_ui.main();
	}
	ui::panel const* environment::ui() const noexcept
	{
		return m_ui.main();
	}
	void environment::expose_inventory(container_component * storage_container)
	{
		auto user_body = m_player ? m_player->linked<body_component>() : nullptr;
		auto user_container = user_body ? user_body->linked<container_component>() : nullptr;
		m_ui.open_storage(storage_container, user_container);
	}

	transform_component *  environment::find_any(point2 const& position)
	{
		transform_component * result = nullptr;

		auto world = m_player ? m_player->world() : nullptr;
		if (world) world->find(position.x(), position.y(), [&result](int /*x*/, int /*y*/, transform_component * obj) { result = obj; });

		return result;
	}
}