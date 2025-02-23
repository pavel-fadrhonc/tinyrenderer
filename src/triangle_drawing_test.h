#include <iostream>

#include "triangle_drawing.h"
#include "geometry.h"
#include "random.h"
#include "constants.h"
#include "my_gl.h"
#include "shader.h"
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

void MatrixInverseTest()
{
	std::array<float, 9> arr{ 2, 2, 3, 4, 5, 6, 7, 8, 9 };
	using Mat3 = MatrixGeneric<float, 3, 3>;
	Mat3 mat {arr};

	using Mat2 = MatrixGeneric<float, 2, 2>;
	auto printCofactor = [](const Mat2& mat)
		{
			for (int i = 0; i < 2; i++)
			{
	 			for (int j = 0; j < 2; j++)
					std::cout << mat.GetElement(i, j);

				std::cout << "\n";
			}
		};

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			std::cout << "Cofactor " << i << j << "\n";

			auto cofMat = mat.GetCofactor(i, j);
			printCofactor(cofMat);
			float determinant = cofMat.determinant();
			std::cout << "Determinant: " << determinant << "\n";

			std::cout << "\n";
		}
	}

	auto adjoint = mat.getAdjoint();

	std::cout << "adjoint matrix:\n";
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			std::cout << adjoint.rawMat[i][j] << ' ';

		std::cout << "\n";
	}

	float determinant = mat.determinant();
	std::cout << "determinant: " << determinant << '\n';

	auto inverse = mat.GetInverse();
	std::cout << inverse << '\n';
}

#define PHONG_SHADER_DEF 1
#define QUANTIZE_SHADER_DEF 2
#define PHONG_NORMAL_SHADER_DEF 3

#define SHADER_DEF PHONG_NORMAL_SHADER_DEF

#if SHADER_DEF == PHONG_SHADER_DEF
inline BasicPhongShader phongShader {};
inline IFragmentShader& fragmentShader = phongShader;
inline IVertexShader& vertexShader = phongShader;
#elif SHADER_DEF == QUANTIZE_SHADER_DEF
constexpr Vec3f QUANTIZE_TINT { 0.34f, 0.58f, 0.19f };
constexpr int QUANTIZE_LEVELS{ 5 };
inline QuantizeShadar quantizeShader( QUANTIZE_TINT, QUANTIZE_LEVELS );
inline IFragmentShader& fragmentShader = quantizeShader;
inline IVertexShader& vertexShader = quantizeShader;
#elif SHADER_DEF == PHONG_NORMAL_SHADER_DEF
inline NormalMappedPhongShader normalPhongShader (100.0f);
inline IFragmentShader& fragmentShader = normalPhongShader;
inline IVertexShader& vertexShader = normalPhongShader;
#endif


void DrawTriangle_Model()
{
	Model headModel{ "../../../assets/models/african_head.obj" };
	constexpr int FAR_PLANE = 1000;
	const Vec3f LIGHT_POS = { 0.f, 0.f, 10.f };
	const Vec3f LIGHT_COLOR = { 1.f, 1.f, 1.f };
	TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, TGAImage::RGB);
	constexpr Vec3f camPos = { 2.0f,1.5f, 10.0f };
	constexpr float scale = 0.85f;
	constexpr Vec2f offsetViewport{ 0.f, 0.f };
	constexpr Vec3f modelPosition{ 0.f, 0.f, 0.f };

	ZBufferBase* zBuffer = new ZBufferIntDefault;
	TGAImage albedoTexture;
	//albedoTexture.read_tga_file("../../../assets/models/african_head_diffuse.tga");
	albedoTexture.read_tga_file("../../../assets/models/african_head_diffuse.tga");
	albedoTexture.flip_vertically();

	TGAImage normalTexture;
	normalTexture.read_tga_file("../../../assets/models/african_head_nm_tangent.tga");
	normalTexture.flip_vertically();
	fragmentShader.SetNormalTexture(&normalTexture);

	// setup necessary matrices
	Mat4 modelMat;
	modelMat.SetIdentity();
	modelMat *= scale;
	modelMat.SetColumn(3, modelPosition.ToPoint());
	Mat4 viewPortMat = getViewport(offsetViewport, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, FAR_PLANE);
	Mat4 projectionMat = getProjection(std::abs(modelPosition.z - camPos.z));
	Mat4 viewMat = getLookAt(camPos, modelPosition);

	Mat4 MVP = viewPortMat * projectionMat * viewMat * modelMat;

	Mat4 MVP_IT = MVP.GetInverse().GetTranspose();

	// setup globals
	mgl::MVP = MVP;
	mgl::MVP_IT = MVP_IT;
	mgl::VP = projectionMat * viewMat;
	mgl::ModelMat = modelMat;
	mgl::ViewMat = viewMat;
	mgl::ProjectionMat = projectionMat;
	mgl::LightDir = LIGHT_POS;
	mgl::LightDir.normalize();
	mgl::LightDirColor = LIGHT_COLOR;
	mgl::CameraPos = camPos;

	vertexShader.SetModel(&headModel);
	fragmentShader.SetAlbedoTexture(&albedoTexture);

	Mat4 M_IT = modelMat.GetInverse().GetTranspose();

	// for each face get all the triangle data and render
	const int numFaces = headModel.nfaces();
	for (int i = 0; i < numFaces; i++) 
	{
		std::vector<int> face = headModel.face(i);
		Vec3f v0 = headModel.vert(face[0]);
		Vec3f v1 = headModel.vert(face[2]);
		Vec3f v2 = headModel.vert(face[4]);

		Vec3f triangleNormal = (v1 - v0).cross(v2 - v0).normalize();
		const Vec3f triangleNormalWS = (modelMat * triangleNormal.ToDirection()).ToVec3().normalize();

		float shading = triangleNormalWS.dot((camPos - v0).normalize());

		if (shading < 0.0f) // backface culling
			continue;

		TGAColor tint = TGAColor::FromFloat( 1.0f, 1.0f, 1.0f, 1.0f);

		Vec3f transV0 = vertexShader.vertex(i, 0);
		Vec3i transV0i = Vec3i{ (int)transV0.x, (int)transV0.y, (int)transV0.z, };

		Vec3f transV1 = vertexShader.vertex(i, 1);
		Vec3i transV1i = Vec3i{ (int)transV1.x, (int)transV1.y, (int)transV1.z, };

		Vec3f transV2 = vertexShader.vertex(i, 2);
		Vec3i transV2i = Vec3i{ (int)transV2.x, (int)transV2.y, (int)transV2.z, };

		Vec3f v0ws = (modelMat * v0.ToPoint()).FromHomogeneous();
		Vec3f v1ws = (modelMat * v1.ToPoint()).FromHomogeneous();
		Vec3f v2ws = (modelMat * v2.ToPoint()).FromHomogeneous();

		constexpr int PERFORMANCE_TEST_ITERATIONS = 1;

		Triangle t
		{
			transV0i, transV1i,transV2i,		
			v0ws, v1ws, v2ws
		};

		for (int perfi = 0; perfi < PERFORMANCE_TEST_ITERATIONS; perfi++)
			DrawTriangleMethod3_WithZ_WithTexture(t, image, tint, FAR_PLANE, *zBuffer, fragmentShader);

		//printf("Drawn face %d out of %d, progress: %.2f %% \n", i, numFaces, (static_cast<float>(i) / numFaces) * 100);
	}


	image.flip_vertically();
	image.write_tga_file("AfricanHead_render_Triangles.tga");
}
