// name: environment.main

#define SAVE_READABLE 0
#define SAVE_BINARY 1

#if SAVE_BINARY
#define SAVE_ARCHIVE <cereal/archives/binary.hpp>
#endif
#if SAVE_READABLE
#define SAVE_ARCHIVE <cereal/archives/xml.hpp>
#endif

#include "environment.hpp"

#include "perception.hpp"
#include "es/unit_builder.hpp"
#include "es/unit_component.hpp"

#include <px/ui/panel.hpp>

#include SAVE_ARCHIVE
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/map.hpp>
#include <json.hpp>
#include <fstream>

namespace {
	char const* quicksave_path = "quicksave.sav";
	const float popup_speed = 0.2f;
	const float movement_speed = 5.0f;
}

namespace px {

	environment::~environment()
	{
		end();
		m_map.resize({ 0,0 });
	}

	environment::environment()
		: m_factory(std::make_unique<factory>())
		, m_player(nullptr)
		, m_run(true)
		, m_turn(1)
		, m_last_turn(0)
		, m_last_time(0)
	{
		m_factory->characters()->provide_environment(this);
	}

	bool environment::running() const noexcept
	{
		return m_run;
	}

	void environment::shutdown() noexcept
	{
		m_run = false;
	}

	void environment::end()
	{
		m_units.clear();
		impersonate(nullptr);
	}
	void environment::impersonate(transform_component * player)
	{
		m_player = player;
		m_factory->sprites()->assign_camera(m_player);
		m_ui.assign_player(m_player);
		m_ui.assign_inventory(m_player ? m_player->linked<body_component, container_component>() : nullptr);
	}
	void environment::update(perception & view, double time)
	{
		// track timestamp from last turn update
		if (m_turn != m_last_turn) {
			m_last_turn = m_turn;
			m_last_time = time;
		}
		double span = time - m_last_time;
		double delta_time = std::min(span * movement_speed, 1.0);

		// notifications
		vector2 camera = m_player ? m_player->interpolate(delta_time) : vector2(0, 0);
		view.clear_popups();
		for (auto const& kv_list : m_notifications) {
			vector2 position = vector2(kv_list.first) - camera;
			for (auto const& note : kv_list.second) {
				float x = static_cast<float>(position.x());
				float y = static_cast<float>(position.y() + span);
				view.emplace_popup(x, y, note.text, note.tint.dissolved(std::min(span * popup_speed, 1.0)));
			}
		}

		// sprite batches
		m_factory->sprites()->update(delta_time);
		view.assign_batches(&m_factory->sprites()->batches());

		// compose user interface
		view.canvas().cls();
		m_ui.main()->layout(view.canvas().range());
		m_ui.main()->draw(view.canvas());
	}
	void environment::turn_begin()
	{
		m_notifications.clear();
		m_visuals.clear();
		for (auto & unit : m_units) {
			auto * transform = unit->transform();
			if (transform) transform->store_position();
		}
	}
	void environment::turn_end()
	{
		m_factory->npc()->fixed_update(1);
		for (auto & vfx : m_visuals) {
			if (vfx.link) {
				vfx.transform.place(vfx.link->position());
			}
		}
		++m_turn;
	}
	template <typename Action>
	void environment::turn(Action && intent_action, int duration)
	{
		turn_begin();
		intent_action();
		for (int i = 0; i < time; ++i) {
			turn_end();
		}
	}
	void environment::target(point2 relative_world_coordinates)
	{
		m_hover = relative_world_coordinates + (m_player ? m_player->position() : point2(0, 0));
		auto target = find_any(m_hover);

		m_ui.assign_target(target, m_hover);
	}
	void environment::step(point2 const& direction)
	{
		if (!m_player) return;

		point2 destination = m_player->position() + direction;

		if (!m_map.traversable(destination)) return;

		auto blocking = find_any(destination);

		if (!blocking) {
			turn_begin();
			m_player->place(destination);
			turn_end();
		}
	}
	void environment::use(unsigned int action_index)
	{
		auto * user = m_player ? m_player->linked<body_component>() : nullptr;
		auto * character = user ? user->linked<character_component>() : nullptr;

		if (auto * skill = character ? character->get_skill(action_index - 1) : nullptr) {

			//int turns = skill->state().duration();

			turn_begin();
			if (skill->targeted()) {
				auto * transform = find_any(m_hover);
				auto * target = transform ? transform->linked<body_component>() : nullptr;

				skill->try_use(user, target);

			}
			else {
				skill->try_use(user, m_hover);
			}
			turn_end();
		}
	}
	void environment::activate(unsigned int /* mod */)
	{
		auto * target = find_any(m_hover);
		auto * body = target ? target->linked<body_component>() : nullptr;
		auto * user = m_player ? m_player->linked<body_component>() : nullptr;

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
		auto character = builder.add_character();

		if (name == "@") builder.add_player();
		if (name == "m_snail") builder.add_npc();

		// setup
		transform->store_position();
		body->health()->set(100);
		body->set_name(name);
		body->set_tag(name);
		body->set_description("mobile template");
		for (int i = 0; i != 10; ++i) {
			auto itm = std::make_shared<rl::item>();
			itm->set_name("item #" + std::to_string(i));
			itm->set_tag("itm_ore");
			itm->set_description("item template");
			itm->make_stacking();
			itm->add(rl::item::enhancement_type::integer(rl::effect::ore_power, i));
			container->add(itm);
		}
		character->learn_skill("sk_v_melee");
		character->learn_skill("sk_s_smite");
		character->learn_skill("sk_s_rend");
		character->learn_skill("sk_s_flurry");
		character->learn_skill("sk_i_pain");
		character->learn_skill("sk_v_teleport");

		rl::item weapon;
		weapon.set_name("weapon");
		weapon.add(rl::item::enhancement_type::integer(rl::effect::weapon_damage, 6));
		body->equip(rl::equipment_slot::weapon_main, weapon);

		// add to scene
		auto result = builder.assemble();
		result->enable();
		m_units.push_back(result);

		return result;
	}

	void environment::start()
	{
		// terrain
		m_map.assigns_sprites(m_factory->sprites());
		m_map.resize({ 100, 100 });
		m_map.generate();
		m_map.dump("data/terrain/level0.dat");

		// units

		spawn("m_snail", { 40, 49 });
		spawn("m_goblin", { 52, 48 });

		for (int i = 54; i != 58; ++i) spawn("p_bookshelf", { i, 46 });
		for (int i = 59; i != 61; ++i) spawn("p_bookshelf", { i, 46 });
		spawn("p_bookshelf", { 54, 47 });
		spawn("p_bookshelf", { 54, 48 });
		spawn("p_bookshelf", { 54, 50 });
		for (int i = 54; i != 61; ++i) spawn("p_bookshelf", { i, 51 });

		spawn("p_box", { 58, 50});

		// player
		auto player = spawn("@", { 55, 47 });

		impersonate(player->transform());
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
#if SAVE_READABLE
		std::ifstream output(name);
		cereal::XMLOutputArchive archive(output);
#endif
#if SAVE_BINARY
		std::ofstream output(name, std::ios::binary);
		cereal::BinaryOutputArchive archive(output);
#endif

		archive(m_turn);
		save_units(archive);
	}
	void environment::load(std::string const& name)
	{
#if SAVE_READABLE
		std::ifstream input(name);
		cereal::XMLInputArchive archive(input);
#endif
#if SAVE_BINARY
		std::ifstream input(name, std::ios::binary);
		cereal::BinaryInputArchive archive(input);
#endif

		end();

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
	template <typename Archive, typename Builder>
	void environment::load_unit(Builder & builder, Archive & archive)
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

	transform_component * environment::find_any(point2 const& position)
	{
		transform_component * result = nullptr;

		auto world = m_player ? m_player->world() : nullptr;
		if (world) world->find(position.x(), position.y(), [&result](int /* x */, int /* y */, transform_component * obj) { result = obj; });

		return result;
	}

	void environment::popup(point2 location, std::string text, color tint, float size)
	{
		m_notifications[location].push_back({ text, tint, size });
	}

	void environment::visual(std::string const& tag, point2 start, point2 destination, transform_component const* follow)
	{
		m_visuals.push_back({ m_factory->sprites()->make_unique(tag),{ destination, start }, follow });
		auto & vfx = m_visuals.back();

		vfx.sprite->connect(&vfx.transform);
		vfx.sprite->activate();
	}
	std::tuple<int, int, bool, bool> environment::hit(body_component const& attacker, body_component const& /* versus */) const
	{
		int magnitude = 0;
		int variant = 0;
		bool is_hit = false;
		bool is_critical = false;

		if (attacker.equipped(rl::equipment_slot::weapon_main)) {
			auto damage_enhancement = attacker.at(rl::equipment_slot::weapon_main).accumulate<rl::effect::weapon_damage>();

			magnitude = damage_enhancement.value0;
			variant = damage_enhancement.subtype;
			is_hit = true;
			is_critical = true;
		}

		return std::tuple<int, int, bool, bool>{ magnitude, variant, is_hit, is_critical };
	}
}