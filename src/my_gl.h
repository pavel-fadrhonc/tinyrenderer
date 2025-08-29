#pragma once
#include "geometry.h"

namespace sor
{
	// values valid in shaders
	inline Mat4 MVP;
	inline Mat4 MVP_IT;
	inline Mat4 ModelMat;
	inline Mat4 ViewMat;
	inline Mat4 ViewportMat;
	inline Mat4 ProjectionMat;
	inline Mat4 VP;

	// for point light
	inline Vec3f LightPos;
	// for directional light
	inline Vec3f LightDir; // normalized
	inline Vec3f LightDirColor;

	inline Vec3f CameraPos;
}
