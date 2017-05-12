#pragma once

namespace px {

	class wrap_unit
	{
	public:
		wrap_unit(body_component * body)
			: m_body(body)
		{

		}
	private:
		body_component * m_body;
	};

}