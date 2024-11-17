#include <array>
#include <algorithm>

#include "geometry.h"
#include "Instrumentor.h"
#include "tgaimage.h"
#include "line_drawing.h"
#include "colors.h"
#include "line_drawing_test.h"

Vec3i ModelSpaceToImageSpace(const Vec3f& vecMS, const Vec2i& imageDimensions)
{
	float widthHalf = imageDimensions.x * 0.5f;
	float heightHalf = imageDimensions.y * 0.5f;
	return {
		static_cast<int>((vecMS.x + 1.0f) * widthHalf),
		static_cast<int>((vecMS.y + 1.0) * heightHalf),
		0
	};
}

// use the line sweeping technique where you go for each pixel of bottom line you draw a line to the top pixel
void DrawTriangleMethod1(const Vec3i& v1, const Vec3i& v2, const Vec3i& v3, TGAImage& image, const TGAColor& color)
{
	/*if (v1.y < v2.y)
	{
		if (v1.y < v3.y)
		{
			b1 = v1;
		}
		else 
		{
			b1 = v3;
		}
	}
	else if (v2.y < v3.y)
	{
		b1 = v2;
	}
	else
	{
		b1 = v3;
	}*/

	const Vec2i imageDimensions{ image.get_width(), image.get_height() };

	//std::array vertices{
	//	ModelSpaceToImageSpace(v1, imageDimensions),
	//	ModelSpaceToImageSpace(v2, imageDimensions),
	//	ModelSpaceToImageSpace(v3, imageDimensions)};

	std::array vertices{v1, v2,	v3};

	std::sort(vertices.begin(), vertices.end(), [](const Vec3i& a, const Vec3i& b) { return a.y < b.y; });
	Vec3i& b1 = vertices[0];	// most bottom point
	Vec3i& b2 = vertices[1];	// second most bottom point
	Vec3i& b3 = vertices[2];	// top point

	auto DrawLineFromBottomSide = [&b3](int x, int y, TGAImage& image, TGAColor color)
		{
			DrawLine(x, y, b3.x, b3.y, image, color);
		};

	ForEachLinePixel(b1.x, b1.y, b2.x, b2.y, image, color, DrawLineFromBottomSide);
}

void DrawTriangleTest()
{
	constexpr int WIDTH = 1000;
	constexpr int HEIGHT = 1000;
	TGAImage image(1000, 1000, TGAImage::RGB);

	// find out bbox
	//Vec2i min{ static_cast<int>((std::min({v1.x, v2.x, v3.x}) + 1.0) * WIDTH / 2),
	//	static_cast<int>((std::min({v1.y, v2.y, v3.y}) + 1.0) * HEIGHT / 2) };
	//
	//Vec2i max{ static_cast<int>((std::max({v1.x, v2.x, v3.x}) + 1.0) * WIDTH / 2),
	//	static_cast<int>((std::max({v1.y, v2.y, v3.y}) + 1.0) * HEIGHT / 2) };

	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	DrawTriangleMethod1(t0[0], t0[1], t0[2], image, red);
	DrawTriangleMethod1(t1[0], t1[1], t1[2], image, white);
	DrawTriangleMethod1(t2[0], t2[1], t2[2], image, green);


	image.flip_vertically();
	image.write_tga_file("triangle_test1.tga");

}

int main(int argc, char** argv)
{
	bEngine::Instrumentor::Get().BeginSession("TinyRenderer");

	DrawTriangleTest();
	//DrawModel();
	//DrawLineTest3();
	//DrawLineTest2();
	

	bEngine::Instrumentor::Get().EndSession();

	return 0;
}