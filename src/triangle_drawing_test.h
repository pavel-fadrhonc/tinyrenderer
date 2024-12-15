#include "triangle_drawing.h"
#include "geometry.h"
#include "random.h"
#include "constants.h"
#include "tgaimage.h"
#include "transformations.h"

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

	DrawTriangleMethod3_WithZ({ t1[0], t1[1], t1[2] } , image, white, FAR_PLANE, zbuffer);
	DrawTriangleMethod3_WithZ({ t0[0], t0[1], t0[2] }, image, red, FAR_PLANE, zbuffer);
	DrawTriangleMethod3_WithZ({ t2[0], t2[1], t2[2] }, image, green, FAR_PLANE, zbuffer);

	image.flip_vertically();
	//image.write_tga_file("triangle_test2.tga");
	image.write_tga_file("triangle_test3.tga");

}

void DrawTriangle_Model()
{
	Model headModel{ "../../../assets/models/african_head.obj" };
	constexpr int FAR_PLANE = 1000;
	TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, TGAImage::RGB);
	ZBufferBase* zBuffer = new ZBufferIntDefault;
	TGAImage texture;
	texture.read_tga_file("../../../assets/models/african_head_diffuse.tga");
	texture.flip_vertically();

	const int numFaces = headModel.nfaces();
	for (int i = 0; i < numFaces; i++) 
	{
		std::vector<int> face = headModel.face(i);
		Vec3f v0 = headModel.vert(face[0]);
		Vec3f v1 = headModel.vert(face[2]);
		Vec3f v2 = headModel.vert(face[4]);

		// apply perspective projection
		constexpr float camZPos = 3.0f;
		auto applyPerspective = [camZPos](const Vec3f& vertex) -> Vec3f
			{
				float perspDivisor = (1.0f - vertex.z / camZPos);
				return
				{
					vertex.x / perspDivisor,
					vertex.y / perspDivisor,
					vertex.z / perspDivisor,
				};
			};
		//v0 = applyPerspective(v0);
		//v1 = applyPerspective(v1);
		//v2 = applyPerspective(v2);

		Vec2f uv0 = headModel.uv(face[1]);
		Vec2f uv1 = headModel.uv(face[3]);
		Vec2f uv2 = headModel.uv(face[5]);

		Vec3f normal = (v2 - v0).cross(v1 - v0).normalize();
		float shading = normal.dot(VBack);

		if (shading < 0.0f) // backface culling
			continue;

		TGAColor tint = TGAColor::FromFloat( shading, shading, shading, 1.0f);
		constexpr float scale = 0.95f;
		constexpr Vec2f offsetNDC{ 0.f, 0.f };

		Mat4 viewPortMat = getViewport(scale, offsetNDC, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, FAR_PLANE);
		Mat4 projectionMat = getProjection(camZPos);


		Vec3f transV0 = (viewPortMat * projectionMat * v0.ToPoint()).FromHomogeneous();
		Vec3i transV0i = Vec3i{ (int)transV0.x, (int)transV0.y, (int)transV0.z, };

		Vec3f transV1 = (viewPortMat * projectionMat * v1.ToPoint()).FromHomogeneous();
		Vec3i transV1i = Vec3i{ (int)transV1.x, (int)transV1.y, (int)transV1.z, };

		Vec3f transV2 = (viewPortMat * projectionMat * v2.ToPoint()).FromHomogeneous();
		Vec3i transV2i = Vec3i{ (int)transV2.x, (int)transV2.y, (int)transV2.z, };


		//Vec3i v0i = ConvertModelCoordsIntoImageCoords(v0,scale, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_X, FAR_PLANE);
		//Vec3i v1i = ConvertModelCoordsIntoImageCoords(v1,scale, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_X, FAR_PLANE);
		//Vec3i v2i = ConvertModelCoordsIntoImageCoords(v2,scale, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_X, FAR_PLANE);

		constexpr int PERFORMANCE_TEST_ITERATIONS = 1;

		Triangle t
		{
			//v0i, v1i, v2i,
			transV0i, transV1i,transV2i,
			v0, v1, v2,
			uv0, uv1, uv2
		};

		for (int perfi = 0; perfi < PERFORMANCE_TEST_ITERATIONS; perfi++)
			DrawTriangleMethod3_WithZ_WithTexture(t, image, tint, FAR_PLANE, *zBuffer, texture);
			//DrawTriangleMethod3_2DCoords(v0i, v1i, v2i, image, color);

		//printf("Drawn face %d out of %d, progress: %.2f %% \n", i, numFaces, (static_cast<float>(i) / numFaces) * 100);
	}


	image.flip_vertically();
	image.write_tga_file("AfricanHead_render_Triangles.tga");
}
