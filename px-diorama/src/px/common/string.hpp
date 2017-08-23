// name: string.hpp
// type: c++ header
// desc: string functions
// auth: is0urce

#pragma once

// string functions
// uft8 enumeration and convertion

#include <string>
#include <functional>

namespace {
	const unsigned int replacement_character = 0xfffd; // '<?>' glyph
	const unsigned int byte_order_mark = 0xfeff; // we should skip byte order mark
}

namespace px {

	class string
	{
	public:
		static bool is_roman(unsigned int codepoint) noexcept
		{
			return codepoint <= 0x7f;
		}
		static std::string code_to_utf8(unsigned int codepoint) noexcept
		{
			std::string out;

			if (codepoint <= 0x7f)
				out.append(1, static_cast<char>(codepoint));
			else if (codepoint <= 0x7ff) {
				out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
				out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
			}
			else if (codepoint <= 0xffff) {
				out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
				out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
				out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
			}
			else {
				out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
				out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
				out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
				out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
			}
			return out;
		}
		// Operator should provide void operator()(unsigned int)
		template <typename Operator>
		static void enumerate_utf8(std::string const& text, Operator && fn)
		{
			std::string::size_type i{};
			auto len = text.length();
			while (i < len)	{
				unsigned char ch = text[i];
				unsigned int letter = replacement_character;

				if (ch < 192) {
					letter = ch;
					++i;
				}
				else if (i + 1 < len) {
					unsigned char ch2 = text[i + 1];
					if (ch < 224 && ch2 > 127) {
						letter = ((ch & 0x1f) << 6) | (ch2 & 0x3f);
						i += 2;
					}
					else if (i + 2 < len) {
						unsigned char ch3 = text[i + 2];
						if (ch < 240 && ch2 > 127 && ch3 > 127) {
							letter = ((ch & 0x0f) << 12) | ((ch2 & 0x3f) << 6) | (ch3 & 0x3f);
							i += 3;
						}
						else if (i + 3 < len) {
							unsigned char ch4 = text[i + 3];
							if (ch < 248 && ch2 > 127 && ch3 > 127 && ch4 > 127) {
								letter = ((ch & 0x07) << 18) | ((ch2 & 0x3f) << 12) | ((ch3 & 0x3f) << 6) | (ch4 & 0x3f);
								i += 4;
							}
							else {
								++i; // that's an error in decoding
							}
						}
						else {
							return;
						}
					}
					else {
						return;
					}
				}
				else {
					return;
				}

				// skip byte-order-mark
				if (letter != byte_order_mark) {
					fn(letter);
				}
			}
		}
	};
}