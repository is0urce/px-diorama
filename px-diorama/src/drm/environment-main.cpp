#include "environment.hpp"

namespace px {

	environment::~environment()
	{
	}

	environment::environment()
		: m_factory(&m_sprites)
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
}