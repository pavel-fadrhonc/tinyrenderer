#pragma once

#include "Instrumentor.h"
#include "tgaimage.h"
#include "model.h"
#include "math.h"
#include "line_drawing.h"
#include "colors.h"

namespace sor
{
	void inline DrawLineTest3()
	{
		PROFILE_FUNCTION();

		TGAImage image(100, 100, Texture::ETextureFormat::RGB);

		constexpr int NUM_LINES = 20;
		constexpr float ANGLE_STEP = 2 * PI / NUM_LINES;
		constexpr int LINE_LENGTH = 50;

		float angle = 0.f;
		constexpr Vector2 center = { .x = 50.f, .y = 50.f };

		constexpr int ITER_COUNT = 1;

		for (int iter = 0; iter < ITER_COUNT; iter++)
		{
			for (int lineIdx = 0; lineIdx < NUM_LINES; lineIdx++, angle += ANGLE_STEP)
			{
				int x = center.x + std::sin(angle) * LINE_LENGTH;
				int y = center.y + std::cos(angle) * LINE_LENGTH;

				DrawLine(center.x, center.y, x, y, image.GetTexture(), red);
			}
		}

		image.flip_vertically();
		image.write_tga_file("output.tga");
	}

	void inline DrawLineTest2()
	{
		TGAImage image(100, 100, Texture::ETextureFormat::RGB);

		DrawLine(13, 20, 80, 40, image.GetTexture(), white);
		DrawLine(20, 13, 40, 80, image.GetTexture(), red);
		DrawLine(80, 40, 13, 20, image.GetTexture(), red);

		image.flip_vertically();
		image.write_tga_file("output.tga");
	}

	void inline DrawModel_wireframe()
	{
		Model headModel{};
		headModel.Load("../../../assets/models/african_head.obj");
		constexpr int WIDTH = 1000;
		constexpr int HEIGHT = 1000;
		TGAImage image(1000, 1000, Texture::ETextureFormat::RGB);

		for (int i = 0; i < headModel.nfaces(); i++) {
			std::vector<int> face = headModel.face(i);
			for (int j = 0; j < 3; j++) {
				Vec3f v0 = headModel.vert(face[j]);
				Vec3f v1 = headModel.vert(face[(j + 1) % 3]);
				int x0 = (v0.x + 1.) * WIDTH / 2.;
				int y0 = (v0.y + 1.) * HEIGHT / 2.;
				int x1 = (v1.x + 1.) * WIDTH / 2.;
				int y1 = (v1.y + 1.) * HEIGHT / 2.;
				DrawLine(x0, y0, x1, y1, image.GetTexture(), white);
			}
		}


		image.flip_vertically();
		image.write_tga_file("AfricanHead_render_wireframe.tga");
	}
}