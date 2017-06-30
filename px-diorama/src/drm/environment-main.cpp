// name: environment-main.cpp
// type: c++

#include "environment.hpp"

#include "perception.hpp"
#include "es/unit.hpp"
#include "es/factory.hpp"
#include "es/unit_builder.hpp"

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
		, m_player(nullptr)
		, m_run(true)
		, m_turn(1)
		, m_last_turn(0)
		, m_last_time(0)
		, m_repository(current_repository)
	{
		m_factory->sprites()->set_cropping(crop_far);
		m_factory->characters()->provide_environment(this);
		start();

		m_repository.reset();
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
			m_terrain.focus(player->position());
		}
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

		if (!m_terrain.traversable(destination)) return;

		transform_component * blocking = find_any(destination);

		if (!blocking) {
			turn_begin();

			m_terrain.wait();
			m_terrain.focus(destination);
			m_player->place(destination);
			//m_terrain.dump();

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

	void environment::spawn(unit_ptr mobile)
	{
		mobile->enable();
		m_units.push_back(mobile);
	}

	void environment::start()
	{
		// terrain
		m_terrain.assigns_sprites(m_factory->sprites());

		// units

		spawn(create_dummy("m_snail", { 0, 9 }));
		spawn(create_dummy("m_mermaid", { 12, 8 }));
		spawn(create_dummy("p_bookshelf", { 1, 1 }));
		spawn(create_dummy("p_box", { 8, 0 }));

		// player
		auto player = create_dummy("m_gnome", { 5, 5 });
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
			is_critical = false;
		}

		return { magnitude, variant, is_hit, is_critical };
	}

	std::shared_ptr<unit> environment::create_dummy(std::string const& name, point2 location)
	{
		// create
		unit_builder builder(m_factory.get());
		auto transform = builder.add_transform(location);
		auto body = builder.add_body();
		auto sprite = builder.add_sprite(name);
		auto container = builder.add_container();
		auto storage = builder.add_storage();
		auto character = builder.add_character();

		if (name == "m_gnome") builder.add_player();
		if (name == "m_snail") builder.add_npc();

		// setup
		transform->store_position();
		body->health()->set(100);
		body->set_name(name);
		body->set_tag(name);
		body->set_description("mobile template");
		for (int i = 0; i != 10; ++i) {
			auto itm = std::make_shared<rl::item>();
			itm->set_name("mossairum");
			itm->make_stacking();
			itm->add(rl::item::enhancement_type::integer(rl::effect::ore_power, 1));
			itm->add(rl::item::enhancement_type::integer(rl::effect::essence, 1024));
			container->add(itm);
		}
		character->learn_skill("sk_v_melee");
		character->learn_skill("sk_s_smite");
		character->learn_skill("sk_s_rend");
		character->learn_skill("sk_s_flurry");
		character->learn_skill("sk_i_pain");
		character->learn_skill("sk_v_teleport");
		character->learn_skill("sk_o_export");
		character->learn_skill("sk_o_import");

		rl::item weapon;
		weapon.set_name("weapon");
		weapon.add(rl::item::enhancement_type::integer(rl::effect::weapon_damage, 6));
		weapon.add(rl::item::enhancement_type::integer(rl::effect::equipment, static_cast<uint32_t>(rl::equipment_slot::weapon_main)));
		body->equip(rl::equipment_slot::weapon_main, weapon);

		return builder.assemble();
	}

	environment::terrain_type & environment::terrain()
	{
		return m_terrain;
	}
}