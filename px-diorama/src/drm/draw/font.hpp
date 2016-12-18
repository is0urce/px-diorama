// name: font.hpp

#pragma once

#include <msdf/msdfgen.h>
#include <msdf/ext/import-font.h>

#include <string>
#include <stdexcept>

namespace px
{
	struct glyph
	{
		unsigned int codepoint;
		float sx, sy, dx, dy;
	};
	class font final
	{
	public:
		void load(unsigned int codepoint)
		{
			msdfgen::Shape shape;
			if (!msdfgen::loadGlyph(shape, m_font, codepoint)) throw std::runtime_error("px::font::load - can't load glyph #" + std::to_string(codepoint));

			shape.normalize();
			msdfgen::Bitmap<float> lum(m_resolution, m_resolution);
			msdfgen::generateSDF(lum, shape, 4.0, 1.0, { 4.0, 4.0 });

			auto & g = m_map[codepoint];
			g.codepoint = codepoint;

			for (int j = 0, h = lum.height(); j != h; ++j)
			{
				for (int i = 0, w = lum.width(); i != w; ++i)
				{
					m_atlas[(m_pen_y + j) * m_width + (m_pen_x + i)] = lum(i, j);
				}
			}

			m_dirty = true;
		}
		glyph const& operator[](unsigned int codepoint)
		{
			auto it = m_map.find(codepoint);
			if (it == m_map.end()) load(codepoint);

			return m_map[codepoint];
		}
		bool dirty() const noexcept
		{
			return m_dirty;
		}
		const float* update(int & width, int & height) noexcept
		{
			width = m_width;
			height = m_height;
			m_dirty = false;
			return m_atlas.data();
		}

	private:
		void resize(unsigned int power)
		{
			m_power = power;
			m_width = 1 << power;
			m_height = 1 << power;
			m_atlas.resize(m_width * m_height);
			m_dirty = true;
		}
		void quadruple()
		{
			resize(m_power + 1);
			m_map.clear();
			m_pen_x = 0;
			m_pen_y = 0;
		}

	public:
		font(std::string const& path, unsigned int resolution_power, unsigned int bitmap_power)
			: m_power(bitmap_power)
			, m_resolution(1 << resolution_power)
		{
			resize(bitmap_power);

			m_library = msdfgen::initializeFreetype();
			if (!m_library) throw std::runtime_error("px::font - init freetype fails");
			m_font = msdfgen::loadFont(m_library, path.c_str());
			if (!m_font) throw std::runtime_error("px::font - font '" + path + "' load fails");
		}
		~font()
		{
			if (m_font)
			{
				msdfgen::destroyFont(m_font);
			}
			if (m_library)
			{
				msdfgen::deinitializeFreetype(m_library);
			}
		}

	private:
		msdfgen::FreetypeHandle * m_library;
		msdfgen::FontHandle * m_font;

		unsigned int m_resolution;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_power;
		std::vector<float> m_atlas;
		std::map<unsigned int, glyph> m_map;
		bool m_dirty;
		unsigned int m_pen_x;
		unsigned int m_pen_y;
	};
}