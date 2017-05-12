// name: ft_face.hpp

#pragma once

#include "ft_library.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdexcept>
#include <string>

namespace px
{
	class ft_face
	{
	public:
		operator FT_Face() const noexcept
		{
			return m_face;
		}
		FT_Face operator->() const noexcept
		{
			return m_face;
		}

	public:
		//void point_size(unsigned int points, unsigned int dpi)
		//{
		//	auto error = FT_Set_Char_Size(
		//		m_face,    /* handle to face object           */
		//		0,       /* char_width in 1/64th of points  */
		//		points * 64,   /* char_height in 1/64th of points */
		//		dpi,     /* horizontal device resolution    */
		//		0);   /* vertical device resolution      */
		//	if (error) throw std::runtime_error("px::rft::ft_face::points_size()");
		//}
		void pixel_size(unsigned int pixels)
		{
			m_size = pixels;
			auto error = FT_Set_Pixel_Sizes(m_face,	0, pixels);
			if (error) throw std::runtime_error("px::rft::ft_face::pixel_size() - FT_Set_Pixel_Sizes error");
		}
		unsigned int size() const noexcept
		{
			return m_size;
		}
		//auto kerning(unsigned int l1, unsigned int l2) const
		//{
		//	FT_Vector kerning;
		//	FT_Get_Kerning(m_face, l1, l2, FT_KERNING_DEFAULT, &kerning);
		//	return (kerning.x >> 6) / double(m_size);
		//}

	public:
		~ft_face()
		{
			FT_Done_Face(m_face);
		}
		ft_face(FT_Library library, char const* path, unsigned int index)
		{
			auto error = FT_New_Face(library, path, index, &m_face);
			if (error) throw std::runtime_error("px::rft::ft_face::ctor() - error #" + std::to_string(error) + " path=" + std::string(path) + " index=" + std::to_string(index));
		}
		ft_face(FT_Library library, char const* path)
			: ft_face(library, path, 0)
		{
		}
		ft_face(ft_face const&) = delete;
		ft_face& operator=(ft_face const&) = delete;

	private:
		FT_Face			m_face;
		unsigned int	m_size;
	};
}