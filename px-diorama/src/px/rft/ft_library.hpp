// name: ft_library.hpp

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <stdexcept>
#include <string>

namespace px
{
	class ft_library
	{
	public:
		operator FT_Library() const noexcept
		{
			return m_library;
		}
		void swap(ft_library & that) noexcept
		{
			std::swap(m_library, that.m_library);
		}
	public:
		ft_library()
		{
			auto error = FT_Init_FreeType(&m_library);
			if (error) throw std::runtime_error("px::rft::ft_library::ctor() - .. an error occurred during library initialization ...");
		}
		ft_library(ft_library const&) = delete;
		ft_library& operator=(ft_library const&) = delete;
		~ft_library()
		{
			FT_Done_FreeType(m_library);
		}
	private:
		FT_Library m_library;
	};
}