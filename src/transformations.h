#pragma once
#include "geometry.h"

// offset is against the center in normalized device [-1, 1] coordinates
inline Mat4 getViewport(float scaleNDC, Vec2f offsetNDC, const int width, const int height, const int farPlane)
{
	return Mat4
	{
		Vec4f { scaleNDC * width * 0.5f, 0.f, 0.f, offsetNDC.x * width + width * 0.5f },
		Vec4f { 0.f, scaleNDC * height * 0.5f, 0.f, offsetNDC.y * height + height * 0.5f },
		Vec4f { 0.f, 0.f, scaleNDC * farPlane * 0.5f, farPlane * 0.5f + farPlane * 0.5f },
		Vec4f { 0.f, 0.f, 0.f, 1.f }
	};
}

inline Mat4 getProjection(float cameraDistance)
{
	Mat4 projMat;
	projMat.SetIdentity();
	projMat.SetElement(3, 2, -1.0f / cameraDistance);
	return projMat;
}