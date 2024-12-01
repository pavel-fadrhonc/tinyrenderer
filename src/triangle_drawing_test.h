#include "triangle_drawing.h"
#include "geometry.h"
#include "random.h"
#include "constants.h"

void DrawTriangleTest()
{
	constexpr int FAR_PLANE = 100;
	TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, TGAImage::RGB);


	Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
	Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
	Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

	//DrawTriangleMethod3_2DCoords(t1[0], t1[1], t1[2], image, white);
	//DrawTriangleMethod3_2DCoords(t0[0], t0[1], t0[2], image, red);
	//DrawTriangleMethod3_2DCoords(t2[0], t2[1], t2[2], image, green);

	ZBuffer2D zbuffer;

	DrawTriangleMethod3_WithZ(t1[0], t1[1], t1[2], image, white, FAR_PLANE, zbuffer);
	DrawTriangleMethod3_WithZ(t0[0], t0[1], t0[2], image, red, FAR_PLANE, zbuffer);
	DrawTriangleMethod3_WithZ(t2[0], t2[1], t2[2], image, green, FAR_PLANE, zbuffer);

	image.flip_vertically();
	//image.write_tga_file("triangle_test2.tga");
	image.write_tga_file("triangle_test3.tga");

}

Vec3i ConvertModelCoordsIntoImageCoords(const Vec3f& vert, const int width, const int height, const int farPlane)
{
	return Vec3i
	{
		static_cast<int>((vert.x + 1.) * width * 0.5f),
		static_cast<int>((vert.y + 1.) * height * 0.5f),
		static_cast<int>((vert.z + 1.) * farPlane * 0.5f)
	};
}

void DrawTriangle_Model()
{
	Model headModel{ "../../../assets/models/african_head.obj" };
	constexpr int FAR_PLANE = 1000;
	TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, TGAImage::RGB);
	ZBufferBase* zBuffer = new ZBufferIntDefault;

	const int numFaces = headModel.nfaces();
	for (int i = 0; i < numFaces; i++) 
	{
		std::vector<int> face = headModel.face(i);
		Vec3f v0 = headModel.vert(face[0]);
		Vec3f v1 = headModel.vert(face[1]);
		Vec3f v2 = headModel.vert(face[2]);

		Vec3f normal = (v2 - v0).cross(v1 - v0).normalize();
		float shading = normal.dot(VBack);

		if (shading < 0.0f) // backface culling
			continue;

		TGAColor color = TGAColor::FromFloat( shading, shading, shading, 1.0f);

		Vec3i v1i = ConvertModelCoordsIntoImageCoords(v1, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_X, FAR_PLANE);
		Vec3i v2i = ConvertModelCoordsIntoImageCoords(v2, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_X, FAR_PLANE);
		Vec3i v0i = ConvertModelCoordsIntoImageCoords(v0, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_X, FAR_PLANE);

		constexpr int PERFORMANCE_TEST_ITERATIONS = 1;

		for (int i = 0; i < PERFORMANCE_TEST_ITERATIONS; i++)
			DrawTriangleMethod3_WithZ(v0i, v1i, v2i, image, color, FAR_PLANE, *zBuffer);
			//DrawTriangleMethod3_2DCoords(v0i, v1i, v2i, image, color);

		printf("Drawn face %d out of %d, progress: %.2f %% \n", i, numFaces, (static_cast<float>(i) / numFaces) * 100);
	}


	image.flip_vertically();
	image.write_tga_file("AfricanHead_render_Triangles.tga");
}
