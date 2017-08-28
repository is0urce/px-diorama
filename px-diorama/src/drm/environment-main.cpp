// name: environment-main.cpp
// type: c++

#include "environment.hpp"

#include "perception.hpp"
#include "es/unit.hpp"
#include "es/factory.hpp"
#include "es/unit_builder.hpp"

#include "rl/tile_terrain.hpp"
#include "vfx.hpp"

#include <px/ui/panel.hpp>

namespace {
	const float popup_speed = 0.2f;
	const float movement_speed = 5.0f;
	const float crop_far = 15.0f;
}

namespace px {

	environment::~environment()
	{
		end();
	}

	environment::environment()
		: m_factory(std::make_unique<factory>())
		, m_terrain(std::make_unique<terrain_type>())
		, m_vfx(std::make_unique<std::vector<vfx>>())
		, m_editor(editor_mode)
		, m_player(nullptr)
		, m_run(true)
		, m_turn(1)
		, m_last_turn(0)
		, m_last_time(0)
		, m_base(base_repository)
		, m_repository(current_repository, &m_base)
	{
		// factory
		m_factory->sprites()->set_cropping(crop_far);
		m_factory->provide_environment(this);

		// terrain
		m_terrain->provide_sprites(m_factory->sprites());
		m_terrain->on_leave([&](point2 const& cell) { archive_scene(cell); });
		m_terrain->on_enter([&](point2 const& cell) { restore_scene(cell); });

		start();

		m_repository.reset();

		std::random_device rd;
		m_rng = std::mt19937(rd());
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
		m_factory->update(delta_time);
		view.assign_batches(&m_factory->sprites()->batches());

		// compose user interface
		view.canvas().cls();
		m_ui.main()->layout(view.canvas().range());
		m_ui.main()->draw(view.canvas());
	}
	void environment::turn_begin()
	{
		m_notifications.clear();
		m_vfx->clear();

		for (auto & unit : m_units) {
			transform_component * transform = unit->transform();
			if (transform) transform->store_position();
		}

		m_terrain->wait();
	}
	void environment::turn_pass(unsigned int pass_turns)
	{
		m_factory->fixed_update(pass_turns);
		m_turn += pass_turns;
	}
	void environment::turn_end()
	{
		for (auto & vfx : *m_vfx) {
			if (vfx.link) {
				vfx.transform.place(vfx.link->position());
			}
		}

		if (m_editor) {
			m_terrain->dump();
		}
	}

	unsigned int environment::distance(point2 const& a, point2 const& b) const noexcept
	{
		return a.king_distance(b);
	}

	bool environment::traversable(point2 const& position, traverse_type traverse) const
	{
		bool block = false;
		auto world = m_player ? m_player->world() : nullptr;
		if (world) world->find(position.x(), position.y(), [&](int /* x */, int /* y */, transform_component * obj) {
			body_component * body = obj ? obj->linked<body_component>() : nullptr;
			block |= body && !body->mass().traversable(traverse);
		});

		return !block && m_terrain->traversable(position, traverse);
	}
	bool environment::transparent(point2 const& position) const
	{
		bool block = false;
		auto world = m_player ? m_player->world() : nullptr;
		if (world) world->find(position.x(), position.y(), [&](int /* x */, int /* y */, transform_component * obj) {
			body_component * body = obj ? obj->linked<body_component>() : nullptr;
			block |= body && !body->mass().transparent();
		});

		return !block && m_terrain->transparent(position);
	}

	bool environment::running() const noexcept
	{
		return m_run;
	}

	void environment::shutdown() noexcept
	{
		m_run = false;
	}

	bool environment::editor() const noexcept
	{
		return m_editor;
	}

	void environment::end()
	{
		m_ui.close_transactions();
		m_ui.break_links();
		impersonate(nullptr);
		m_units.clear();
	}
	void environment::impersonate(transform_component * player)
	{
		m_player = player;
		m_factory->sprites()->assign_camera(m_player);
		m_ui.assign_incarnation(m_player);

		if (m_player) {
			m_terrain->focus(player->position());
		}
	}
	transform_component const* environment::player() const noexcept
	{
		return m_player;
	}

	template <typename Action>
	void environment::turn(Action && intent_action, int duration)
	{
		turn_begin();
		intent_action();
		turn_pass(duration);
		turn_end();
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
		body_component * body = m_player->linked<body_component>();

		if (!body) return;
		if (!traversable(destination, body->traverse())) return;

		turn_begin();

		m_player->place(destination);

		turn_pass(1);
		turn_end();

		m_terrain->focus(destination);
	}
	void environment::use(unsigned int action_index)
	{
		auto * user = m_player ? m_player->linked<body_component>() : nullptr;
		auto * character = user ? user->linked<character_component>() : nullptr;

		if (auto * skill = character ? character->get_skill(action_index - 1) : nullptr) {

			bool skill_used = false;
			int duration = skill->state().duration();

			turn_begin();
			if (skill->targeted()) {
				transform_component * transform = find_any(m_hover);
				body_component * target = transform ? transform->linked<body_component>() : nullptr;

				skill_used = skill->try_use(user, target);
			}
			else {
				skill_used = skill->try_use(user, m_hover);
			}
			turn_end();

			if (skill_used && duration > 0) {
				turn_pass(duration);
			}
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

	void environment::spawn(unit_ptr mobile)
	{
		px_assert(mobile);
		transform_component * transform = mobile->transform();
		px_assert(transform);

		if (transform) {
			transform->store_position();
		}

		mobile->enable();
		m_units.push_back(mobile);
	}
	void environment::spawn(unit_ptr mobile, point2 location)
	{
		px_assert(mobile);
		transform_component * transform = mobile->transform();
		px_assert(transform);
		
		if (transform) {
			transform->place(location);
			transform->store_position();
		}

		mobile->enable();
		m_units.push_back(mobile);
	}
	void environment::edit(unit_ptr mobile)
	{
		m_edited = mobile;
	}
	environment::unit_type * environment::edited()
	{
		return m_edited.get();
	}

	void environment::start()
	{
		// units

		spawn(create_dummy("p_vein", { 0, 5 }));
		spawn(create_dummy("m_rat", { 3, 3 }));
		spawn(create_dummy("p_bag", { 0, 3 }));

		// player
		auto player = create_dummy("m_gnome", { 2, 2 });
		spawn(player);

		impersonate(player->transform());
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
		auto container = m_player ? m_player->linked<body_component, container_component>() : nullptr;
		m_ui.open_storage(storage_container, container);
	}
	void environment::open_workshop(unsigned int /* workshop */)
	{
		auto container = m_player ? m_player->linked<body_component, container_component>() : nullptr;
		m_ui.open_workshop(container);
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
		m_vfx->push_back({ m_factory->sprites()->make_unique(tag),{ destination, start }, follow });
		auto & vfx = m_vfx->back();

		vfx.sprite->connect(&vfx.transform);
		vfx.sprite->activate();
	}
	std::tuple<int, int, bool, bool> environment::hit(body_component const& attacker, body_component const& /* versus */)
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
			is_critical = std::uniform_int_distribution<int>{ 1, 20 }(m_rng) == 20;
		}

		return { magnitude, variant, is_hit, is_critical };
	}
	void environment::damage(body_component & body, int damage)
	{
		auto & hp = body.health();

		if (hp) {
			hp->damage(damage);
	
			if (transform_component * transform = body.linked<transform_component>()) {
				popup(transform->position(), std::to_string(damage), color(m_player != transform ? 0xffcc00 : 0xff0000), 1.0f);
			}
		}
	}

	environment::unit_ptr environment::create_empty(std::string const& tag, point2 location)
	{
		unit_builder builder(m_factory.get());

		auto transform = builder.add_transform(location);
		auto sprite = builder.add_sprite("e_dummy");
		auto body = builder.add_body();

		body->set_name(tag);
		body->set_tag(tag);

		auto result = builder.assemble();

		return result;
	}
	environment::unit_ptr environment::create_dummy(std::string const& name, point2 location)
	{
		// create
		unit_builder builder(m_factory.get());
		auto transform = builder.add_transform(location);
		auto sprite = builder.add_sprite(name);
		auto body = builder.add_body();
		auto container = builder.add_container();
		auto character = builder.add_character();

		if (name == "m_gnome") {
			builder.add_player();

			body->join_faction(1);

			character->learn_skill("sk_v_melee");
			character->learn_skill("sk_s_smite");
			character->learn_skill("sk_s_rend");
			character->learn_skill("sk_s_flurry");
			character->learn_skill("sk_i_pain");
			character->learn_skill("sk_v_teleport");
			character->learn_skill("sk_o_export");
			character->learn_skill("sk_o_import");

			for (unsigned int i = 0; i != 10; ++i) {
				auto itm = std::make_shared<rl::item>();
				itm->set_name("iron");
				itm->make_stacking();
				itm->add(rl::item::enhancement_type::integer(rl::effect::ore_power, 1));
				itm->add(rl::item::enhancement_type::integer(rl::effect::essence, 1024));
				container->add(itm);
			}
		}
		else {
			builder.add_npc();

			character->learn_skill("sk_v_melee");
		}

		// setup
		body->set_name(name);
		body->set_tag(name);
		body->health().create(100);
		body->traverse().make_traversable<rl::traverse::floor>();

		body_component::buff_type intrinsic;
		intrinsic.set_name("intrinsic");
		intrinsic.set_tag("default");
		intrinsic.add(rl::enhancement<rl::effect>::integer(rl::effect::accuracy, 85));
		intrinsic.add(rl::enhancement<rl::effect>::integer(rl::effect::dodge, 5));
		intrinsic.add(rl::enhancement<rl::effect>::integer(rl::effect::hp_bonus, 100));
		body->buffs().push_back(intrinsic);

		rl::item weapon;
		weapon.set_name("weapon");
		weapon.add(rl::item::enhancement_type::integer(rl::effect::weapon_damage, 6));
		weapon.add(rl::item::enhancement_type::integer(rl::effect::equipment, static_cast<uint32_t>(rl::equipment_slot::weapon_main)));
		body->equip(rl::equipment_slot::weapon_main, weapon);

		return builder.assemble();
	}

	environment::terrain_type & environment::terrain()
	{
		return *m_terrain;
	}

	void environment::loot(body_component * user, container_component * inventory)
	{
		container_component * bag = user ? user->linked<container_component>() : nullptr;

		if (inventory && bag) {
			inventory->transfer(*bag);
		}
	}
}