#pragma once

#include "geometry.h"

namespace sor
{
	struct TGAColor {
		union {
			struct {
				unsigned char b, g, r, a;
			};
			unsigned char raw[4];
			unsigned int val;
		};
		int bytespp;

		constexpr TGAColor() : val(0), bytespp(1) {}

		constexpr TGAColor(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {}

		static inline TGAColor FromFloat(const float R, const float G, const float B, const float A)
		{
			return TGAColor(static_cast<unsigned char>(R * 255), static_cast<unsigned char>(G * 255), static_cast<unsigned char>(B * 255), static_cast<unsigned char>(A * 255));
		}

		static inline TGAColor FromVec4(const Vec4f& vec)
		{
			return FromFloat(vec.x(), vec.y(), vec.z(), vec.w());
		}

		constexpr TGAColor(int v, int bpp) : val(v), bytespp(bpp) {}

		constexpr TGAColor(const TGAColor& c) : val(c.val), bytespp(c.bytespp) {}

		constexpr TGAColor(const unsigned char* p, int bpp) : val(0), bytespp(bpp) {
			for (int i = 0; i < bpp; i++) {
				raw[i] = p[i];
			}
		}

		TGAColor& operator =(const TGAColor& c) {
			if (this != &c) {
				bytespp = c.bytespp;
				val = c.val;
			}
			return *this;
		}

		void scale(float scalar)
		{
			r = (char)((float)r * scalar);
			g = (char)((float)g * scalar);
			b = (char)((float)b * scalar);
			a = (char)((float)a * scalar);
		}

		Vec4f ToFloat()
		{
			return Vec4f{ static_cast<float>(r) / 255.0f, static_cast<float>(g) / 255.0f, static_cast<float>(b) / 255.0f, static_cast<float>(a) / 255.0f };
		}

		TGAColor operator*(const TGAColor& c)
		{
			TGAColor col{ *this };
			col *= c;
			return col;
		}

		TGAColor& operator*=(const TGAColor& c)
		{
			r = (char)((float)r / 255.0f * c.r);
			g = (char)((float)g / 255.0f * c.g);
			b = (char)((float)b / 255.0f * c.b);
			a = (char)((float)a / 255.0f * c.a);
			return *this;
		}
	};
}