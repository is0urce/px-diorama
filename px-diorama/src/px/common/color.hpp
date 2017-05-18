// name: color.hpp
// type: c++ header
// desc: class with implementation
// auth: is0urce

// color with real rgba components
// + and * operators change alpha components

#pragma once

#include <cmath> // sin, cos for colorspace transforms

namespace px {

	class color
	{
	public:
		typedef double component;

		// attributes
	public:
		component R;
		component G;
		component B;
		component A;

		// ctor & dtor
	public:
		constexpr color() noexcept
			: R{}
			, G{}
			, B{}
			, A(1) {}
		constexpr color(component r, component g, component b) noexcept
			: R(r)
			, G(g)
			, B(b)
			, A(1) {}
		constexpr color(component r, component g, component b, component a) noexcept
			: R(r)
			, G(g)
			, B(b)
			, A(a) {}
		constexpr color(unsigned int hex) noexcept
			: R(hex / 256 / 256 % 256 / 255.0)
			, G(hex / 256 % 256 / 255.0)
			, B(hex % 256 / 255.0)
			, A(1) {}

	public:
		static constexpr color rgb(unsigned int r, unsigned int g, unsigned int b) noexcept { return color(r / 255.0, g / 255.0, b / 255.0); }
		static constexpr color rgba(unsigned int r, unsigned int g, unsigned int b, unsigned int a) noexcept { return color(r / 255.0, g / 255.0, b / 255.0, a / 255.0); }
		static constexpr color black() noexcept { return color(0, 0, 0, 1); };
		static constexpr color white() noexcept { return color(1, 1, 1, 1); };

		// operators

		color operator-() const noexcept { return color(-R, -G, -B, A); };
		color& operator+=(color const& rh) noexcept { R += rh.R; G += rh.G; B += rh.B; A += rh.A; return *this; };
		color& operator-=(color const& rh) noexcept { R -= rh.R; G -= rh.G; B -= rh.B; A -= rh.A; return *this; };
		color& operator*=(color const& rh) noexcept { R *= rh.R, G *= rh.G, B *= rh.B, A *= rh.A; return *this; };

		bool operator==(color const& rh) const noexcept { return R == rh.R && G == rh.G && B == rh.B && A == rh.A; }
		bool operator!=(color const& rh) const noexcept { return !(*this == rh); }
		color operator+(color c) const noexcept { c += *this; return c; }
		color operator-(color c) const noexcept { c -= *this; return c; }
		color operator*(color c) const noexcept { c *= *this; return c; }

		color operator*(component c) const noexcept { return color(R * c, G * c, B * c, A * c); };
		color operator/(component c) const { return color(R / c, G / c, B / c, A / c); };
		color operator*=(component c) noexcept { *this = *this * c; return *this; };
		color operator/=(component c) { *this = *this / c; return *this; };

		void set_rgba(unsigned int r, unsigned int g, unsigned int b, unsigned int a) noexcept { R = r / 255.0; G = g / 255.0; B = b / 255.0; A = a / 255.0; }
		void set_rgb(unsigned int r, unsigned int g, unsigned int b) noexcept { R = r / 255.0; G = g / 255.0; B = b / 255.0; }
		void set_hex(unsigned int hex) noexcept { set_rgb(hex / 256 / 256 % 256, hex / 256 % 256, hex % 256); };
		void shift_hue(double angle) { *this = transform_hue(*this, angle); };
		void shift_brightness(double scale) { *this = transform_hsv(*this, 0, 1.0, scale); }
		void shift_hsv(double hue, double saturation, double v) { *this = transform_hsv(*this, hue, saturation, v); };
		color transform_hsv(double hue, double saturation, double v) const { return transform_hsv(*this, hue, saturation, v); };
		color average(color other) const noexcept { other += *this; other /= 2; return other; };
		double luminance() const noexcept { return 0.2125 * R + 0.7154 * G + 0.0721 * B; };
		color lerp(color b, component t) const noexcept { b *= t; b += *this * (t - 1); return b; }
		void multiple_rgb(double multiplier) noexcept { R *= multiplier; G *= multiplier; B *= multiplier; }
		color& multiplied_rgb(double multiplier) const noexcept { auto result = *this; result.multiple_rgb(multiplier); return result; }
		color dissolved(component w) const noexcept { auto result = *this; result.A *= (1 - w); return result; }

		// hsv transformation
		// hue - hue shift (in degrees) in hardcoded 'default' colorspace preset
		// saturation - saturation multiplier (scalar), v - value multiplier (scalar)
		static color transform_hsv(color const& in, double hue, double saturation, double V)
		{
			double VSU = V * saturation * std::cos(hue);
			double VSW = V * saturation * std::sin(hue);

			color ret(in);
			ret.R = (.299*V + .701*VSU + .168*VSW)*in.R
				+ (.587*V - .587*VSU + .330*VSW)*in.G
				+ (.114*V - .114*VSU - .497*VSW)*in.B;
			ret.G = (.299*V - .299*VSU - .328*VSW)*in.R
				+ (.587*V + .413*VSU + .035*VSW)*in.G
				+ (.114*V - .114*VSU + .292*VSW)*in.B;
			ret.B = (.299*V - .3*VSU + 1.25*VSW)*in.R
				+ (.587*V - .588*VSU - 1.05*VSW)*in.G
				+ (.114*V + .886*VSU - .203*VSW)*in.B;

			return ret;
		}
		static color transform_hue(color const& in, double angle)
		{
			double U = std::cos(angle);
			double W = std::sin(angle);

			color ret(in);
			ret.R = (.299 + .701*U + .168*W) * in.R
				+ (.587 - .587*U + .330*W) * in.G
				+ (.114 - .114*U - .497*W) * in.B;
			ret.G = (.299 - .299*U - .328*W) * in.R
				+ (.587 + .413*U + .035*W) * in.G
				+ (.114 - .114*U + .292*W) * in.B;
			ret.B = (.299 - .3*U + 1.25*W) * in.R
				+ (.587 - .588*U - 1.05*W) * in.G
				+ (.114 + .886*U - .203*W) * in.B;

			return ret;
		}

		// i-o, we have dedicated repeat function arguments to compliment structures like solid-tinted polygons
		template <typename Memory>
		void write(Memory *memory) const
		{
			memory[0] = static_cast<Memory>(R);
			memory[1] = static_cast<Memory>(G);
			memory[2] = static_cast<Memory>(B);
			memory[3] = static_cast<Memory>(A);
		};
		template <typename Memory>
		void write(Memory *memory, size_t repeat) const
		{
			for (size_t i = 0; i < repeat; ++i)
			{
				write(memory);
				memory += sizeof(*this) / sizeof(component); // plus num of components
			}
		}
		void write(component *memory) const
		{
			memory[0] = R;
			memory[1] = G;
			memory[2] = B;
			memory[3] = A;
		}
		void write(component *memory, size_t repeat) const
		{
			for (size_t i = 0; i < repeat; ++i)
			{
				write(memory);
				memory += sizeof(*this) / sizeof(component); // plus num of components
			}
		}

		// aux
		static color average(color const& a, color const& b) { return (a + b) / 2; }
		static color average(color const& a, color const& b, color const& c) { return (a + b + c) / 3; }
		static color average(color const& a, color const& b, color const& c, color const& d) { return (a + b + c + d) / 4; }

		static const color blue;
	};
}