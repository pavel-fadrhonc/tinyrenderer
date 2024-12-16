#pragma once
#include "geometry.h"

// offset is viewport offset in the image coordinates
inline Mat4 getViewport(Vec2f offset, const int width, const int height, const int farPlane)
{
	return Mat4
	{
		Vec4f { width * 0.5f, 0.f, 0.f, offset.x + width * 0.5f },
		Vec4f { 0.f, height * 0.5f, 0.f, offset.y + height * 0.5f },
		Vec4f { 0.f, 0.f, farPlane * 0.5f, farPlane * 0.5f + farPlane * 0.5f },
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

inline Mat4 getLookAt(const Vec3f& eyePos, const Vec3f& center)
{
	Vec3f forward = (eyePos - center).normalize();
	Vec3f right = VUp.cross(forward).normalize();
	Vec3f up = forward.cross(right).normalize();

	Mat4 lookAtMat;
	lookAtMat.SetRow(0, right);
	lookAtMat.SetRow(1, up);
	lookAtMat.SetRow(2, forward);
	lookAtMat.SetColumn(3, (-center).ToPoint());

	return lookAtMat;
}