#include <valarray>

#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);

constexpr float PI = 3.14159265f;

struct Vector2
{
	float x;
	float y;
};

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	for (float t = 0.; t < 1.; t += .01) {
		int x = x0 + (x1 - x0) * t;
		int y = y0 + (y1 - y0) * t;
		image.set(x, y, color);
	}
}

void line2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) {
	for (int x = x0; x <= x1; x++) {
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1. - t) + y1 * t;
		image.set(x, y, color);
	}
}

void line3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	//if (x0 > x1 || y0 > y1)
	//{
	//	std::swap(x0, x1);
	//	std::swap(y0, y1);
	//}


	int rangeY = y1 - y0;
	int rangeX = x1 - x0;

	const float dx = rangeY == 0 ? 1.f : static_cast<float>(rangeX) / static_cast<float>(rangeY);
	const float dy = rangeX == 0 ? 1.f : static_cast<float>(rangeY) / static_cast<float>(rangeX);

	printf("Starting line from (%d, %d) to (%d, %d); rangeX: %d, rangeY: %d, dx: %f, dy: %f .\n", x0, y0, x1, y1, rangeX, rangeY, dx, dy);

	auto putPoint = [&color, &image](const int x, const int y)
		{
			printf("Putting point (%d, %d)\n", x, y);
			image.set(x, y, color);
		};

	if (std::abs(dx) > std::abs(dy))
	{
		float y = y0;
		int incX = rangeX > 0 ? 1 : -1;
		for (int x = x0; rangeX > 0 ? x < x1 : x > x1; x += incX, y+= dy)
		{
			putPoint(x, static_cast<int>(y));
		}
	}
	else
	{
		float x = x0;
		int incY = rangeY > 0 ? 1 : -1;
		for (int y = y0; rangeY > 0 ? y < y1 : y > y1; y += incY, x += dx)
		{
			putPoint(static_cast<int>(x), y);
		}
	}
}

void DrawPixelTest()
{
	TGAImage image(100, 100, TGAImage::RGB);
	image.set(52, 41, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
}

void DrawLineTest()
{
	TGAImage image(100, 100, TGAImage::RGB);

	line(10, 25, 80, 80, image, { 255, 255, 255, 255 });

	image.flip_vertically();
	image.write_tga_file("output.tga");
}

void DrawLineTest2()
{
	TGAImage image(100, 100, TGAImage::RGB);

	line2(13, 20, 80, 40, image, white);
	line2(20, 13, 40, 80, image, red);
	line2(80, 40, 13, 20, image, red);

	image.flip_vertically();
	image.write_tga_file("output.tga");
}

void DrawLineTest3()
{
	TGAImage image(100, 100, TGAImage::RGB);

	constexpr int NUM_LINES = 20;
	constexpr float ANGLE_STEP = 2 * PI / NUM_LINES;
	constexpr int LINE_LENGTH = 50;

	float angle = 0.f;
	constexpr Vector2 center = { .x= 50.f, .y= 50.f };
	for (int lineIdx = 0; lineIdx < NUM_LINES; lineIdx++, angle += ANGLE_STEP)
	{
		int x = center.x + std::sin(angle) * LINE_LENGTH;
		int y = center.y + std::cos(angle) * LINE_LENGTH;
	
		line3(center.x, center.y, x, y, image, red);
	}


	//line3(50, 50, 0, 99, image, white);

	//line3(13, 20, 80, 40, image, white);
	//line3(20, 13, 40, 80, image, red);
	//line3(80, 40, 13, 20, image, red);

	image.flip_vertically();
	image.write_tga_file("output.tga");
}

int main(int argc, char** argv)
{
	DrawLineTest3();

	return 0;
}