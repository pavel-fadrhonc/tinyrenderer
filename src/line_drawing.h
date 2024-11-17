#pragma once

#include <functional>

#include "tgaimage.h"
#include "Instrumentor.h"

void PutPointToImage(int x, int y, TGAImage& image, TGAColor color)
{
	image.set(x, y, color);
}

template
<typename Tfunc>
void ForEachLinePixel(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color, Tfunc pixelFunc)
{
	PROFILE_FUNCTION()

	int rangeY = y1 - y0;
	int rangeX = x1 - x0;

	const float dx = rangeY == 0 ? 1.f : static_cast<float>(rangeX) / abs(static_cast<float>(rangeY));
	const float dy = rangeX == 0 ? 1.f : static_cast<float>(rangeY) / abs(static_cast<float>(rangeX));

	//printf("Starting line from (%d, %d) to (%d, %d); rangeX: %d, rangeY: %d, dx: %f, dy: %f .\n", x0, y0, x1, y1, rangeX, rangeY, dx, dy);

	if (std::abs(dx) > std::abs(dy))
	{
		float y = y0;
		int incX = rangeX > 0 ? 1 : -1;
		for (int x = x0; rangeX > 0 ? x < x1 : x > x1; x += incX, y += dy)
		{
			pixelFunc(x, static_cast<int>(y), image, color);
		}
	}
	else
	{
		float x = x0;
		int incY = rangeY > 0 ? 1 : -1;
		for (int y = y0; rangeY > 0 ? y < y1 : y > y1; y += incY, x += dx)
		{
			pixelFunc(static_cast<int>(x), y, image, color);
		}
	}
}

/*void ForEachLinePixel(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color, void(*pixelFunc)(int, int, TGAImage&, TGAColor))
{
	PROFILE_FUNCTION()

	int rangeY = y1 - y0;
	int rangeX = x1 - x0;

	const float dx = rangeY == 0 ? 1.f : static_cast<float>(rangeX) / static_cast<float>(rangeY);
	const float dy = rangeX == 0 ? 1.f : static_cast<float>(rangeY) / static_cast<float>(rangeX);

	//printf("Starting line from (%d, %d) to (%d, %d); rangeX: %d, rangeY: %d, dx: %f, dy: %f .\n", x0, y0, x1, y1, rangeX, rangeY, dx, dy);

	if (std::abs(dx) > std::abs(dy))
	{
		float y = y0;
		int incX = rangeX > 0 ? 1 : -1;
		for (int x = x0; rangeX > 0 ? x < x1 : x > x1; x += incX, y += dy)
		{
			pixelFunc(x, static_cast<int>(y), image, color);
		}
	}
	else
	{
		float x = x0;
		int incY = rangeY > 0 ? 1 : -1;
		for (int y = y0; rangeY > 0 ? y < y1 : y > y1; y += incY, x += dx)
		{
			pixelFunc(static_cast<int>(x), y, image, color);
		}
	}
}*/

/*void ForEachLinePixel_Heavy(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color, const std::function<void(int, int, TGAImage&, TGAColor)>& pixelFunc)
{
	PROFILE_FUNCTION()

		int rangeY = y1 - y0;
	int rangeX = x1 - x0;

	const float dx = rangeY == 0 ? 1.f : static_cast<float>(rangeX) / static_cast<float>(rangeY);
	const float dy = rangeX == 0 ? 1.f : static_cast<float>(rangeY) / static_cast<float>(rangeX);

	//printf("Starting line from (%d, %d) to (%d, %d); rangeX: %d, rangeY: %d, dx: %f, dy: %f .\n", x0, y0, x1, y1, rangeX, rangeY, dx, dy);

	if (std::abs(dx) > std::abs(dy))
	{
		float y = y0;
		int incX = rangeX > 0 ? 1 : -1;
		for (int x = x0; rangeX > 0 ? x < x1 : x > x1; x += incX, y += dy)
		{
			pixelFunc(x, static_cast<int>(y), image, color);
		}
	}
	else
	{
		float x = x0;
		int incY = rangeY > 0 ? 1 : -1;
		for (int y = y0; rangeY > 0 ? y < y1 : y > y1; y += incY, x += dx)
		{
			pixelFunc(static_cast<int>(x), y, image, color);
		}
	}
}*/

void DrawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	ForEachLinePixel(x0, y0, x1, y1, image, color, PutPointToImage);
}