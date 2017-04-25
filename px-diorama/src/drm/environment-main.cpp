#include "environment.hpp"

#include <px/ui/board.hpp>
#include <px/ui/text.hpp>
#include <px/ui/button.hpp>
#include <px/ui/toggle_panel.hpp>

#include "ui/inventory_list.hpp"
#include "ui/recipe_list.hpp"
#include "ui/target_panel.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>
#include <fstream>

namespace {
	char const* quicksave_path = "quick.sav";
}

namespace px {

	environment::~environment()
	{
		clear();
	}

	environment::environment()
		: m_factory(std::make_unique<factory>(&m_sprites))
		, m_player(nullptr)
		, m_inventory(nullptr)
		, m_run(true)
	{
		setup_ui();
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
		m_player = nullptr;
	}
	void environment::update(perception & view) const
	{
		// render sprites
		if (auto transform = m_player ? m_player->transform() : nullptr)
		{
			float dx = -static_cast<float>(transform->x());
			float dy = -static_cast<float>(transform->y());
			m_factory->sprites()->write(view.batches(), dx, dy);
		}

		// render user interface
		m_ui.draw(view.canvas());
	}

	std::shared_ptr<unit> environment::spawn(std::string const& name, point2 location)
	{
		unit_builder builder(m_factory.get());
		auto transform = builder.add_transform(location);
		auto body = builder.add_body();
		auto sprite = builder.add_sprite(name);
		auto container = builder.add_container();
		auto storage = builder.add_storage();

		// setup
		for (int i = 0; i != 100; ++i)
		{
			auto itm = std::make_shared<rl::item>();
			itm->set_name("item #" + std::to_string(i));
			itm->add({ rl::effect::ore_power, 0x100, 0x500 });
			container->add(itm);
		}
		body->health().create();
		body->set_name(name);

		// track
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

		m_player = spawn("@", { 54, 46 }).get();

		spawn("m_snake", { 50, 50 });
		spawn("m_h_rabling_harvester", { 50, 51 });
		spawn("m_h_rabling_reaper", { 50, 52 });
		spawn("m_h_rabling_shepard", { 50, 53 });

		spawn("m", { 53, 47 });

		spawn("p_table", { 55, 47 });
		spawn("p_bookshelf", { 54, 47 });
		spawn("p_locker", { 55, 48 });
		spawn("p_box", { 55, 49 });

		// ui
		if (m_inventory) m_inventory->set_container(m_player->transform()->linked<body_component>()->linked<container_component>());
	}
	void environment::generate_terrain()
	{
		m_map.resize({ 100, 100 });
		m_map.enumerate([this](auto const& point, auto & tile) {
			tile.transform.move(point);
			tile.sprite = m_factory->sprites()->make_unique(";");

			tile.sprite->connect(&tile.transform);
			tile.sprite->activate();

			tile.make_wall();
		});

		std::mt19937 rng;
		fn::dig_generator dig(100, 100);
		dig.generate(rng, 5, 12, 1, 15);
		matrix2<unsigned char> map(100, 100);
		dig.rasterize(map);
		map.enumerate([&](auto const& point, unsigned char t) {
			if (t == 0) return;
			auto & tile = m_map[point];

			tile.sprite = m_factory->sprites()->make_unique("+");

			tile.sprite->connect(&tile.transform);
			tile.sprite->activate();

			tile.make_ground();
		});
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

		save_units(archive);
	}
	void environment::load(std::string const& name)
	{
		std::ifstream input(name);

		//cereal::XMLInputArchive archive(input);
		cereal::BinaryInputArchive archive(input);

		clear();

		load_units(archive);
	}


	ui::panel & environment::ui() noexcept
	{
		return m_ui;
	}
	ui::panel const& environment::ui() const noexcept
	{
		return m_ui;
	}
	void environment::layout_ui(rectangle bounds) noexcept
	{
		m_ui.layout(bounds);
	}

	void environment::expose_inventory(container_component * /*inventory*/)
	{
		m_ui["storage"].reverse_toggle();
	}

	void environment::setup_ui()
	{
		//auto inventory_block = m_ui.make<ui::panel>("inventory_block", { {0.25, 0.25}, {0, 1}, {0, -1}, {0.5, 0.5} });
		//inventory_block->make<ui::board>("background", ui::fill, color{ 0, 0, 1, 1 });
		//m_inventory = inventory_block->make<ui::inventory_list>("list", ui::fill).get();

		//auto inventory_toggle = m_ui.make<ui::toggle_panel>("inventory_toggle", { {0.25, 0.25}, {0, 0}, {0, 1}, {0.5, 0.0} });
		//inventory_toggle->add_background({ 0, 0, 0.5, 1 });
		//inventory_toggle->add_label("Inventory");
		//inventory_toggle->assign_content(inventory_block, false);

		//std::list<recipe> recipes;
		//recipes.push_back({ "sword", recipe_type::weapon, 8 });
		//recipes.push_back({ "mace", recipe_type::weapon, 6});
		//recipes.push_back({ "dagger", recipe_type::weapon, 4 });
		//m_ui.make<ui::recipe_list>("recipes", { {0.0, 0.0}, {0,0}, {0,0}, {0.5,0.0} }, std::move(recipes));

		auto storage = m_ui.make<ui::panel>("storage", { { 0.0, 0.0 },{ 0, 1 },{ 0, -1 },{ 0.5, 0.5 } });
		storage->make<ui::board>("background", ui::fill, color{ 1, 1, 0, 0.5 });
		storage->deactivate();

		m_target_panel = m_ui.make<ui::target_panel>("target", { { 1.0, 1.0 },{ -26, -2 },{ 25, 1 },{ 0.0, 0.0 } });

	}
}