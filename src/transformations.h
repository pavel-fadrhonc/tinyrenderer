#pragma once
#include "geometry.h"

namespace sor
{

	// offset is viewport offset in the image coordinates
	inline Mat4 getViewport(Vec2f offset, const int width, const int height, const int farPlane)
	{
		return Mat4
		{
			Vec4f { width * 0.5f, 0.f, 0.f, offset.x + width * 0.5f },
			Vec4f { 0.f, height * 0.5f, 0.f, offset.y + height * 0.5f },
			// Vec4f { 0.f, 0.f, farPlane * 0.5f, farPlane * 0.5f },
			Vec4f { 0.f, 0.f, 1.f, 0.f},
			Vec4f { 0.f, 0.f, 0.f, 1.f }
		};
	}

	inline Mat4 getProjection(float near_, float far_)
	{
		Mat4 projMat;
		projMat.SetRow(0, Vec4f{ near_, 0.f, 0.f, 0.f });
		projMat.SetRow(1, Vec4f{ 0.f, near_, 0.f, 0.f });
		projMat.SetRow(2, Vec4f{ 0.f, 0.f, near_ + far_, -far_ * near_ });
		projMat.SetRow(3, Vec4f{ 0.f, 0.f, 1.f, 0.f });

		return projMat;
	}

	// this is not necessarily lookAt matrix in traditional sense where it would transform model vertices to world space
	// as if the model is looking at the target but rather the inverted version of that - a View matrix that transforms
	// world space into camera space. But instead of constructing the look at and just calling .Inverse() on it,
	// we build the inverted matrix directly by
	// 1) Inverting the rotation matrix - rotation matrix is orthogonal and it's inverse is it's transpose so we put basis
	//		vectors as rows
	// 2) Inverting the translation. And because M.Inverse() = (A*B).Inverse() = B.Inverse() * A.Inverse() we need to
	//		multiply the inverted translating vector (position of the eye frame) with inverted rotation matrix
	// more information about detailed distinction here: https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/lookat-function/framing-lookat-function.html
	inline Mat4 getLookAt(const Vec3f& eyePos, const Vec3f& center)
	{
		Vec3f forward = (center - eyePos).normalize();
		Vec3f right = VUp.cross(forward).normalize();
		Vec3f up = forward.cross(right).normalize();

		Mat4 lookAtMat;
		lookAtMat.SetIdentity();
		lookAtMat.SetRow(0, right);
		lookAtMat.SetRow(1, up);
		lookAtMat.SetRow(2, forward);

		Mat4 translationMat;
		translationMat.SetIdentity();
		translationMat.SetColumn(3, (-eyePos).ToPoint());

		lookAtMat = lookAtMat * translationMat;

		return lookAtMat;
	}

}