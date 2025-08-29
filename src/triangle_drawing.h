#pragma once

#include "shader.h"
#include "z_buffer.h"
#include "line_drawing.h"
#include "geometry.h"
#include <array>
#include <algorithm>

namespace sor
{

	// use the line sweeping technique where you go for each pixel of bottom line you draw a line to the top pixel
	// experimental, doesn't really draw the triangle reliably
	void DrawTriangleMethod1(const Vec3i& v1, const Vec3i& v2, const Vec3i& v3, TGAImage& image, const TGAColor& color);


	void DrawTriangleMethod2(const Vec3i& v1_, const Vec3i& v2_, const Vec3i& v3_, TGAImage& image, const TGAColor& color);

	/// <summary>
	///	Draws the triangle given by 3 coordinates for which it is assumed they all line on z=0 plane	
	/// </summary>
	void DrawTriangleMethod3_2DCoords(const Vec3i& v1_, const Vec3i& v2_, const Vec3i& v3_, TGAImage& image, const TGAColor& color);

	struct Triangle
	{
		// screen space positions before pespective division
		Vec4f v0ss;
		Vec4f v1ss;
		Vec4f v2ss;

		int index;
	};


	// draws just lines between triangle vertices
	void DrawTriangleWired(const Triangle& t, Texture& output, const TGAColor& color);

	// draw using standard method of computing bounding box and then barycentric coordinates for every pixel to find out if it's inside of the triangle
	void DrawTriangle_Standard(const Triangle& t, Texture& outputTex, ZBufferBase& zBuffer, IFragmentShader& fragmentShader);

	/// <summary>
	///	Draws the triangle given by 3 coordinates that have Z coordinate as int
	/// </summary>
	void DrawTriangleMethod3_WithZ_WithTexture(const Triangle& t, Texture& texture, const TGAColor& tint,
		int farPlaneCoord, ZBufferBase& zBuffer, IFragmentShader& fragmentShader);

	/// <summary>
	///	Draws the triangle given by 3 coordinates that have Z coordinate as int
	/// </summary>
	void DrawTriangleMethod3_WithZ(const Triangle& t, TGAImage& image, const TGAColor& color,
		int farPlaneCoord, ZBufferBase& zBuffer);

	void inline triangle_reference(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
		PROFILE_FUNCTION()
			if (t0.y == t1.y && t0.y == t2.y) return; // i dont care about degenerate triangles
		if (t0.y > t1.y) std::swap(t0, t1);
		if (t0.y > t2.y) std::swap(t0, t2);
		if (t1.y > t2.y) std::swap(t1, t2);
		int total_height = t2.y - t0.y;
		for (int i = 0; i < total_height; i++) {
			bool second_half = i > t1.y - t0.y || t1.y == t0.y;
			int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
			float alpha = (float)i / total_height;
			float beta = (float)(i - (second_half ? t1.y - t0.y : 0)) / segment_height; // be careful: with above conditions no division by zero here
			Vec2i A = t0 + (t2 - t0) * alpha;
			Vec2i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
			if (A.x > B.x) std::swap(A, B);
			for (int j = A.x; j <= B.x; j++) {
				image.set(j, t0.y + i, color); // attention, due to int casts t0.y+i != A.y
			}
		}
	}

}

