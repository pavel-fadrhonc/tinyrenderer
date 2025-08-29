#include <iostream>
#include <memory>

#include "triangle_drawing.h"
#include "geometry.h"
#include "random.h"
#include "constants.h"
#include "input.h"
#include "math.h"
#include "my_gl.h"
#include "shader.h"
#include "TGAColor.h"
#include "tgaimage.h"
#include "transformations.h"
#include "time.h"

namespace sor
{

	void inline DrawTriangleTest()
	{
		constexpr int FAR_PLANE = 100;
		TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, Texture::ETextureFormat::RGB);

		Vec2i t0[3] = { Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80) };
		Vec2i t1[3] = { Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180) };
		Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };

		ZBufferDummy zbuffer;

		// DrawTriangleMethod3_WithZ({ t1[0], t1[1], t1[2] } , image, white, FAR_PLANE, zbuffer);
		// DrawTriangleMethod3_WithZ({ t0[0], t0[1], t0[2] }, image, red, FAR_PLANE, zbuffer);
		// DrawTriangleMethod3_WithZ({ t2[0], t2[1], t2[2] }, image, green, FAR_PLANE, zbuffer);

		image.flip_vertically();
		//image.write_tga_file("triangle_test2.tga");
		image.write_tga_file("triangle_test3.tga");

	}

	void inline MatrixInverseTest()
	{
		std::array<float, 9> arr{ 2, 2, 3, 4, 5, 6, 7, 8, 9 };
		using Mat3 = MatrixGeneric<float, 3, 3>;
		Mat3 mat{ arr };

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

	inline const char* OUTPUT_FILE_NAME = "diablo_specular.tga";

	struct DrawContext
	{
		Model model;
		Texture screenTexture;
		std::unique_ptr<ZBufferBase> zBuffer = std::make_unique<ZBufferFloatDefault>();

		// textures
		TGAImage albedoTexture;
		TGAImage normalTexture;
		TGAImage specularTexture;
	};
	inline DrawContext g_DrawContext;

	// Currently this only serves separation into stuff that is done once at the start and stuff that is done every frame
	void inline PrepareForDrawModel()
	{
		g_DrawContext.screenTexture = Texture{ IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, Texture::ETextureFormat::RGB };

		g_DrawContext.model.Load(MODEL_PATHS[(int) SCENE]);

		g_DrawContext.albedoTexture.read_tga_file(ALBEDO_PATHS[(int) SCENE]);
		g_DrawContext.albedoTexture.flip_vertically();

		if (NORMAL_TEXTURE_PATHS[(int) SCENE] != nullptr)
		{
			g_DrawContext.normalTexture.read_tga_file(NORMAL_TEXTURE_PATHS[(int) SCENE]);
			g_DrawContext.normalTexture.flip_vertically();
			fragmentShader.SetNormalTexture(&g_DrawContext.normalTexture);
		}

		if (SPECULAR_TEXTURE_PATHS[(int) SCENE] != nullptr)
		{
			g_DrawContext.specularTexture.read_tga_file(SPECULAR_TEXTURE_PATHS[(int) SCENE]);
			g_DrawContext.specularTexture.flip_vertically();
			fragmentShader.SetSpecularTexture(&g_DrawContext.specularTexture);
		}
		
		ModelMat.SetIdentity();
		ModelMat *= MODEL_SCALE;
		ModelMat.SetColumn(3, MODEL_POSITION.ToPoint());

		ViewportMat = getViewport(VIEWPORT_OFFSET, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, FAR_PLANE);
		ProjectionMat = getProjection(NEAR_PLANE, FAR_PLANE);
		ViewMat = getLookAt(CAMERA_POSITION, MODEL_POSITION);
		MVP = ProjectionMat * ViewMat * ModelMat;
		MVP_IT = MVP.GetInverse().GetTranspose();
		VP = ProjectionMat * ViewMat;
		LightDir = LIGHT_POS;
		LightDir.normalize();
		LightDirColor = LIGHT_COLOR;
		CameraPos = CAMERA_POSITION;

		vertexShader.SetModel(&g_DrawContext.model);
		fragmentShader.SetAlbedoTexture(&g_DrawContext.albedoTexture);
	}

	//--------------------------------------------------------------------------------------------------
	void inline RotateModel()
	{
		static const float rotSpeed = PI / 4.f; // in radians per second

		float rotationDelta = (float) g_DeviceInput.ReadKeyInput(EKeyCodeFlags::F, EInputType::DOWN) * -rotSpeed * GetDeltaTime()
			+ (float) g_DeviceInput.ReadKeyInput(EKeyCodeFlags::S, EInputType::DOWN) * rotSpeed * GetDeltaTime();
		ModelMat.SetYaw(ModelMat.GetYaw() + rotationDelta);
#if 0
		static float lastTimeSeconds = 0;
		// rotate based on time 
		float currentTimeSeconds = GetTimeSinceStartupSeconds();

		float rotationDelta = (currentTimeSeconds - lastTimeSeconds) * rotSpeed;
		ModelMat.SetYaw(ModelMat.GetYaw() + rotationDelta);

		lastTimeSeconds = currentTimeSeconds;
#endif
	}

	//--------------------------------------------------------------------------------------------------
	void inline DrawModel(DrawContext* pDrawContext)
	{
		// the output image in case I ever need it again
		//TGAImage image(IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, TGAImage::RGB);
		// for each face get all the triangle data and render
		const int numFaces = pDrawContext->model.nfaces();
		for (int i = 0; i < numFaces; i++)
		{
			std::vector<int> face = pDrawContext->model.face(i);
			Vec3f v0 = pDrawContext->model.vert(face[0]);
			Vec3f v1 = pDrawContext->model.vert(face[2]);
			Vec3f v2 = pDrawContext->model.vert(face[4]);

			Vec3f triangleNormal = (v1 - v0).cross(v2 - v0).normalize();
			const Vec3f triangleNormalWS = (ModelMat * triangleNormal.ToDirection()).ToVec3().normalize();

			float shading = triangleNormalWS.dot((CAMERA_POSITION - v0).normalize());

			if (shading < 0.0f) // backface culling
				continue;

			TGAColor tint = TGAColor::FromFloat(1.0f, 1.0f, 1.0f, 1.0f);

			Vec4f screenSpacePosV0 = ViewportMat * vertexShader.vertex(i, 0);
			Vec4f screenSpacePosV1 = ViewportMat * vertexShader.vertex(i, 1);
			Vec4f screenSpacePosV2 = ViewportMat * vertexShader.vertex(i, 2);

			Triangle t
			{
				screenSpacePosV0, screenSpacePosV1, screenSpacePosV2,
				i
			};

			g_DrawContext.zBuffer->Clear();
			// DrawTriangleWired(t, g_DrawContext.screenTexture, TGAColor::FromFloat( 1.0f, 1.0f, 1.0f, 0.f ));
			DrawTriangle_Standard(t, g_DrawContext.screenTexture, *g_DrawContext.zBuffer, fragmentShader);
			// DrawTriangleMethod3_WithZ_WithTexture(t, g_DrawContext.screenTexture, tint, FAR_PLANE, *g_DrawContext.zBuffer, fragmentShader);

			//printf("Drawn face %d out of %d, progress: %.2f %% \n", i, numFaces, (static_cast<float>(i) / numFaces) * 100);
		}


		// image.flip_vertically();
		// image.write_tga_file(OUTPUT_FILE_NAME);
	}
}
