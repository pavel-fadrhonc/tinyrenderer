#pragma once
#include "random.h"


/*struct TGAColor2 {
	union {
		struct {
			unsigned char b, g, r, a;
		};
		unsigned char raw[4];
		unsigned int val;
	};
	int bytespp;

	constexpr TGAColor2() : val(0), bytespp(1) {}

	constexpr TGAColor2(unsigned char R, unsigned char G, unsigned char B, unsigned char A) : b(B), g(G), r(R), a(A), bytespp(4) {	}
};*/

//const static inline TGAColor2 white2 = TGAColor2(255, 255, 255, 255);

namespace sor
{
	const static inline TGAColor white = TGAColor(255u, 255u, 255u, 255u);
	const static inline TGAColor red = TGAColor{ 255, 0, 0, 255 };
	const static inline TGAColor green = TGAColor{ 0, 255, 0, 255 };

	TGAColor GetRandomTGAColor()
	{
		return TGAColor{
			static_cast<unsigned char>(Random::get(0, 255)),
			static_cast<unsigned char>(Random::get(0, 255)),
			static_cast<unsigned char>(Random::get(0, 255)),
			static_cast<unsigned char>(1.0f) };
	}
}

