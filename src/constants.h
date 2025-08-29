#pragma once

#include "geometry.h"
#include "shader.h"

namespace sor
{
	constexpr int IMAGE_SIZE_DEFAULT_X = 1000;
	constexpr int IMAGE_SIZE_DEFAULT_Y = 1000;

	constexpr Vec2i IMAGE_SIZE_DEFAULT{ IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y };

	constexpr float FPS = 60.0f;
	constexpr float FRAME_DURATION = 1.f / FPS;
	constexpr float FPS_COUNTER_REFRESH_FREQUENCY = 2.0f; // how many time per second do we refresh FPS counter
#define SHOW_FPS_COUNTER 1

#define PHONG_SHADER_DEF 1
#define QUANTIZE_SHADER_DEF 2
#define PHONG_NORMAL_SHADER_DEF 3
#define FLAT_COLOR_SHADER_DEF 4

//#define SHADER_DEF PHONG_NORMAL_SHADER_DEF
#define SHADER_DEF FLAT_COLOR_SHADER_DEF

#if SHADER_DEF == PHONG_SHADER_DEF
	inline BasicPhongShader phongShader{};
	inline IFragmentShader& fragmentShader = phongShader;
	inline IVertexShader& vertexShader = phongShader;
#elif SHADER_DEF == QUANTIZE_SHADER_DEF
	constexpr Vec3f QUANTIZE_TINT{ 0.34f, 0.58f, 0.19f };
	constexpr int QUANTIZE_LEVELS{ 5 };
	inline QuantizeShadar quantizeShader(QUANTIZE_TINT, QUANTIZE_LEVELS);
	inline IFragmentShader& fragmentShader = quantizeShader;
	inline IVertexShader& vertexShader = quantizeShader;
#elif SHADER_DEF == PHONG_NORMAL_SHADER_DEF
	inline NormalMappedPhongShader normalPhongShader(10.0f);
	inline IFragmentShader& fragmentShader = normalPhongShader;
	inline IVertexShader& vertexShader = normalPhongShader;
#elif SHADER_DEF == FLAT_COLOR_SHADER_DEF
	inline FlatColorShader flatColorShader;
	inline IFragmentShader& fragmentShader = flatColorShader;
	inline IVertexShader& vertexShader = flatColorShader;
#endif

	inline const char* AFRICAN_HEAD_MODEL_PATH = "../../../assets/models/african_head.obj";
	inline const char* AFRICAN_HEAD_DIFFUSE_PATH = "../../../assets/models/african_head_diffuse.tga";
	inline const char* AFRICAN_HEAD_NORMAL_TANGENT = "../../../assets/models/african_head_nm_tangent.tga";

	inline const char* DIABLO_POSE_MODEL_PATH = "../../../assets/models/diablo3_pose.obj";
	inline const char* DIABLO_POSE_DIFFUSE_PATH = "../../../assets/models/diablo3_pose_diffuse.tga";
	inline const char* DIABLO_POSE_NORMAL_TANGENT_PATH = "../../../assets/models/diablo3_pose_nm_tangent.tga";
	inline const char* DIABLO_POSE_GLOW_PATH = "../../../assets/models/diablo3_pose_glow.tga";
	inline const char* DIABLO_POSE_SPEC_PATH = "../../../assets/models/diablo3_pose_spec.tga";

	inline const char* BOX_PATH = "../../../assets/models/box.obj";
	inline const char* BOX_ALBEDO_PATH = "../../../assets/models/chess.tga";

	inline const char* PLANE_PATH = "../../../assets/models/plane.obj";
	inline const char* PLANE_ALBEDO_PATH = "../../../assets/models/chess.tga";

	enum class EScene
	{
		DIABLO_HEAD,
		CHESS_BOX,
		SIMPLE_PLANE,
		AFRICAN_HEAD,
		COUNT
	};

	const EScene SCENE = EScene::SIMPLE_PLANE;

	inline const char* MODEL_PATHS[(int) EScene::COUNT]
	{
		DIABLO_POSE_MODEL_PATH,
		BOX_PATH,
		PLANE_PATH,
		AFRICAN_HEAD_MODEL_PATH
	};

	inline const char* ALBEDO_PATHS[(int) EScene::COUNT]
	{
		DIABLO_POSE_DIFFUSE_PATH,
		BOX_ALBEDO_PATH,
		PLANE_ALBEDO_PATH,
		AFRICAN_HEAD_DIFFUSE_PATH
	};

	inline const char* NORMAL_TEXTURE_PATHS[(int) EScene::COUNT]
	{
		DIABLO_POSE_NORMAL_TANGENT_PATH,
		nullptr,
		AFRICAN_HEAD_NORMAL_TANGENT
	};

	inline const char* SPECULAR_TEXTURE_PATHS[(int) EScene::COUNT]
	{
		DIABLO_POSE_SPEC_PATH,
		nullptr,
		nullptr
	};

	// inline const char* MODEL_PATH = DIABLO_POSE_MODEL_PATH;
	// inline const char* ALBEDO_PATH = DIABLO_POSE_DIFFUSE_PATH;
	// inline const char* NORMAL_TS_PATH = DIABLO_POSE_NORMAL_TANGENT_PATH;
	// inline const char* SPECULAR_TS_PATH = DIABLO_POSE_SPEC_PATH;

	constexpr float FAR_PLANE = 20.f;
	constexpr float NEAR_PLANE = 1.f;
	constexpr Vec3f LIGHT_POS = { 1.f, 1.f, 1.f };
	constexpr Vec3f LIGHT_COLOR = { 1.f, 1.f, 1.f };

	constexpr Vec3f CAMERA_POSITION = { 0.0f,0.f, 3.0f };
	constexpr float MODEL_SCALE = 1.00f;
	constexpr Vec2f VIEWPORT_OFFSET{ 0.f, 0.f };
	constexpr Vec3f MODEL_POSITION{ 0.f, 0.f, 0.f };
}
