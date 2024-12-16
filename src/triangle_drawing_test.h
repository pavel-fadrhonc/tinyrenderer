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
	const Vec3f LIGHT_POS = { 0.f, 0.f, 10.f };
	TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, TGAImage::RGB);
	constexpr Vec3f camPos = { 1.0f, 0.0f, 10.0f };
	constexpr float scale = 0.85f;
	constexpr Vec2f offsetViewport{ 200.f, 0.f };
	constexpr Vec3f modelPosition{ 0.f, 0.f, 0.f };

	ZBufferBase* zBuffer = new ZBufferIntDefault;
	TGAImage texture;
	texture.read_tga_file("../../../assets/models/african_head_diffuse.tga");
	texture.flip_vertically();

	// setup necessary matrices
	Mat4 modelMat;
	modelMat.SetIdentity();
	modelMat *= scale;
	modelMat.SetColumn(3, modelPosition.ToPoint());
	Mat4 viewPortMat = getViewport(offsetViewport, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, FAR_PLANE);
	Mat4 projectionMat = getProjection(std::abs(modelPosition.z - camPos.z));
	Mat4 viewMat = getLookAt(camPos, modelPosition);

	Mat4 MVP = viewPortMat * projectionMat * viewMat * modelMat;

	// for each face get all the triangle data and render
	const int numFaces = headModel.nfaces();
	for (int i = 0; i < numFaces; i++) 
	{
		std::vector<int> face = headModel.face(i);
		Vec3f v0 = headModel.vert(face[0]);
		Vec3f v1 = headModel.vert(face[2]);
		Vec3f v2 = headModel.vert(face[4]);

		Vec2f uv0 = headModel.uv(face[1]);
		Vec2f uv1 = headModel.uv(face[3]);
		Vec2f uv2 = headModel.uv(face[5]);

		Vec3f normalV0 = headModel.vnormal(face[0]);
		Vec3f normalV1 = headModel.vnormal(face[2]);
		Vec3f normalV2 = headModel.vnormal(face[4]);

		Vec3f triangleNormal = (v2 - v0).cross(v1 - v0).normalize();
		float shading = triangleNormal.dot(-LIGHT_POS);

		if (shading < 0.0f) // backface culling
			continue;

		//TGAColor tint = TGAColor::FromFloat( shading, shading, shading, 1.0f);
		TGAColor tint = TGAColor::FromFloat( 1.0f, 1.0f, 1.0f, 1.0f);

		Vec3f transV0 = (MVP * v0.ToPoint()).FromHomogeneous();
		Vec3i transV0i = Vec3i{ (int)transV0.x, (int)transV0.y, (int)transV0.z, };

		Vec3f transV1 = (MVP * v1.ToPoint()).FromHomogeneous();
		Vec3i transV1i = Vec3i{ (int)transV1.x, (int)transV1.y, (int)transV1.z, };

		Vec3f transV2 = (MVP * v2.ToPoint()).FromHomogeneous();
		Vec3i transV2i = Vec3i{ (int)transV2.x, (int)transV2.y, (int)transV2.z, };

		Vec3f v0ws = (modelMat * v0.ToPoint()).FromHomogeneous();
		Vec3f v1ws = (modelMat * v1.ToPoint()).FromHomogeneous();
		Vec3f v2ws = (modelMat * v2.ToPoint()).FromHomogeneous();

		constexpr int PERFORMANCE_TEST_ITERATIONS = 1;

		Triangle t
		{
			transV0i, transV1i,transV2i,
			v0ws, v1ws, v2ws,
			normalV0,normalV1,normalV2,
			uv0, uv1, uv2
		};

		for (int perfi = 0; perfi < PERFORMANCE_TEST_ITERATIONS; perfi++)
			DrawTriangleMethod3_WithZ_WithTexture(t, image, tint, FAR_PLANE, *zBuffer, texture, LIGHT_POS);

		//printf("Drawn face %d out of %d, progress: %.2f %% \n", i, numFaces, (static_cast<float>(i) / numFaces) * 100);
	}


	image.flip_vertically();
	image.write_tga_file("AfricanHead_render_Triangles.tga");
}
