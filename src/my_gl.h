#pragma once
#include "geometry.h"

namespace mgl
{
	// values valid in shaders
	inline Mat4 MVP;
	inline Mat4 ModelMat;
	inline Mat4 ViewMat;
	inline Mat4 ProjectionMat;

	// for point light
	inline Vec3f LightPos;
	// for directional light
	inline Vec3f LightDir; // normalized
	inline Vec3f LightDirColor;
}
