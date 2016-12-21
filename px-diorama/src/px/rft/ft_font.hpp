#pragma once

#include "ft_face.hpp"
#include "glyph.hpp"

#include <map>
#include <vector>

namespace px
{
	class font
	{
	public:
		font(const char* name, unsigned int size)
			: m_lib(), m_face(m_lib, name, size)
		{
			m_face.pixel_size(12);
		}
	private:
		ft_library m_lib;
		ft_face m_face;
		std::map<unsigned int, glyph> m_map;
		std::vector<unsigned char> m_atlas;
	};
}