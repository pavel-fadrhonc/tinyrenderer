
#include "shader.h"
#include "z_buffer.h"
// use the line sweeping technique where you go for each pixel of bottom line you draw a line to the top pixel
// experimental, doesn't really draw the triangle reliably
void DrawTriangleMethod1(const Vec3i& v1, const Vec3i& v2, const Vec3i& v3, TGAImage& image, const TGAColor& color)
{
	/*if (v1.y < v2.y)
	{
		if (v1.y < v3.y)
		{
			b1 = v1;
		}
		else
		{
			b1 = v3;
		}
	}
	else if (v2.y < v3.y)
	{
		b1 = v2;
	}
	else
	{
		b1 = v3;
	}*/

	const Vec2i imageDimensions{ image.get_width(), image.get_height() };

	//std::array vertices{
	//	ModelSpaceToImageSpace(v1, imageDimensions),
	//	ModelSpaceToImageSpace(v2, imageDimensions),
	//	ModelSpaceToImageSpace(v3, imageDimensions)};

	std::array vertices{ v1, v2,	v3 };

	std::sort(vertices.begin(), vertices.end(), [](const Vec3i& a, const Vec3i& b) { return a.y < b.y; });
	Vec3i& b1 = vertices[0];	// most bottom point
	Vec3i& b2 = vertices[1];	// second most bottom point
	Vec3i& b3 = vertices[2];	// top point

	auto DrawLineFromBottomSide = [&b3](int x, int y, TGAImage& image, TGAColor color)
		{
			DrawLine(x, y, b3.x, b3.y, image, color);
		};

	ForEachLinePixel(b1.x, b1.y, b2.x, b2.y, image, color, DrawLineFromBottomSide);
}


void DrawTriangleMethod2(const Vec3i& v1_, const Vec3i& v2_, const Vec3i& v3_, TGAImage& image, const TGAColor& color)
{
	PROFILE_FUNCTION()
	// for each point in triangle bbox test if it is inside by using barycentric coordinates

	Vec3i v1{ v1_ };
	Vec3i v2{ v2_ };
	Vec3i v3{ v3_ };

	// switch if vertices are not in CW order
	Vec3i normal = (v3_ - v1_).cross(v2_ - v1_);
	if (normal.z < 0)
	{
		// right hand rule says that if we take sides of triangles expressed as vectors is this order and
		// the triangle is CW and we are in 2D (Vec3.z == 0) the resulting cross product will have positive z
		// and will point "out the screen". This is consistent with how right-handed 
		// coordinate system is defined.
		// So if the result is not positive we know that we can just switch first and last point and we will make it so.
		std::swap(v1, v3);
		normal.z *= -1; // we don't technically need to normal for this method but might come handy for determining exact
		// u,v and w barycentric coordinates
	}

	// find out bbox
	Vec2i min{ (std::min({v1.x, v2.x, v3.x})),
		(std::min({v1.y, v2.y, v3.y})) };

	Vec2i max{ (std::max({v1.x, v2.x, v3.x})),
		(std::max({v1.y, v2.y, v3.y})) };

	auto isInsideTriangle = [normal, &v1, &v2, &v3](const Vec3i& p)
		{
			// instead of dividing by the area or dot with normal we can take advantage of the fact that we are in 2D
			// which means the cross product will always have only z coordinate axis
			// so if we're pointing not "into the screen" but "out of the screen we know the point is on the wrong side

			// the order is of utmost importance here and it has to be the same as the order which we use to determine
			// the "clockwiseness" of the triangle
			bool uTest = (p - v1).cross(v2 - v1).z > 0;
			bool vTest = (p - v2).cross(v3 - v2).z > 0;
			bool wTest = (p - v3).cross(v1 - v3).z > 0;

			return uTest && vTest && wTest;

			//float bary_a = TriangleArea(v1, p, v2) / areaABC;
			//float bary_b = TriangleArea(v1, p, v3) / areaABC;
			//float bary_c = TriangleArea(v2, p, v3) / areaABC;
			//
			//return bary_a > 0 && bary_b > 0 && bary_c > 0;
		};

	for (int x = min.x; x < max.x; x++)
		for (int y = min.y; y < max.y; y++)
		{
			Vec3i point{ x, y, 0 };
			if (isInsideTriangle(point))
			{
				image.set(x, y, color);
			}
		}
}

/// <summary>
///	Draws the triangle given by 3 coordinates for which it is assumed they all line on z=0 plane	
/// </summary>
void DrawTriangleMethod3_2DCoords(const Vec3i& v1_, const Vec3i& v2_, const Vec3i& v3_, TGAImage& image, const TGAColor& color)
{
	PROFILE_FUNCTION()
	std::array vertices{ v1_, v2_, v3_ };

	// sort to find the most bottom
	std::sort(vertices.begin(), vertices.end(), [](const Vec3i& a, const Vec3i& b) { return a.y < b.y; });
	Vec3i& a = vertices[0];	// most bottom point
	Vec3i& b = vertices[1];	// second most bottom point
	Vec3i& c = vertices[2];	// top point

	// make the triangle vertices go in CCW order
	Vec3i normal = (c - a).cross(b - a);
	if (normal.z > 0)
		std::swap(b, c);

	// for every line determine the start and end with the same approach as in drawing line
	// and then draw all pixels on the x axis in between.
	// Once you hit the end of one of these lines, continue with the other one connected (the third, remaining)

	Vec2i firstLine = (c - a).ToVec2();
	Vec2i secondLine = (b - a).ToVec2();

	// this the amount of x pixels added when we advance one row up
	float dxFirst = firstLine.y > 0 ? static_cast<float>(firstLine.x) / static_cast<float>(firstLine.y) : 0;
	float dxSecond = secondLine.y > 0 ? static_cast<float>(secondLine.x) / static_cast<float>(secondLine.y) : 0;

	const int OUT_OF_BOUNDS_LINE = image.get_height() + 1;
	int topLine = OUT_OF_BOUNDS_LINE;

	// we start from vertex a both and end line but eventually one of these will change depending on
	// if line a->c ends sooner or a->b ends sooner
	int lineEndXReference = a.x;
	int lineStartXReference = a.x;

	int lineEndYReference = a.y;
	int lineStartYReference = a.y;

	// once we have a as most bottom and vertices in CCW order we always draw from the a->c line to the a->b line
	// then, depending on whether b.y is lower then c.y the a->c line switches to c->b line or a->b line switches to b->c line
	// and the starting points for start or end line has to be adjusted accordingly
	for (int line = a.y; line < topLine; )
	{
		// translate from absolute image space into relative triangle space
		int lineStartYRelative = line - lineStartYReference;
		int lineEndYRelative = line - lineEndYReference;

		// determine the start and end of the line
		int xStart = lineStartXReference + static_cast<int>(dxFirst * static_cast<float>(lineStartYRelative));
		int xEnd = lineEndXReference + static_cast<int>(dxSecond * static_cast<float>(lineEndYRelative));

		// draw the whole line
		for (int x = xStart; x <= xEnd; x++)
		{
			image.set(x, line, color);
		}

		line++;
		if (topLine == OUT_OF_BOUNDS_LINE)
		{
			if (line > c.y)
			{
				Vec2i thirdLine = (b - c).ToVec2();
				float dxThird = static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y);

				dxFirst = dxThird;
				topLine = b.y;
				lineStartXReference = c.x;
				lineStartYReference = c.y;
			}
			else if (line > b.y)
			{
				Vec2i thirdLine = (c - b).ToVec2();
				float dxThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y) : 0;

				dxSecond = dxThird;
				topLine = c.y;
				lineEndXReference = b.x;
				lineEndYReference = b.y;
			}
		}
	}
}

struct Triangle
{
	// screen space positions
	Vec3f v1ss;
	Vec3f v2ss;
	Vec3f v3ss;

	// world space positions
	// Vec3f v1ws;
	// Vec3f v2ws;
	// Vec3f v3ws;
};

/// <summary>
///	Draws the triangle given by 3 coordinates that have Z coordinate as int
/// </summary>
void DrawTriangleMethod3_WithZ_WithTexture(const Triangle& t, TGAImage& image, const TGAColor& tint,
	int farPlaneCoord, ZBufferBase& zBuffer, IFragmentShader& fragmentShader)
{
#define USE_INTS_FOR_TEXTURING 1

	PROFILE_FUNCTION()
	std::array vertices{ t.v1ss, t.v2ss, t.v3ss };

	// sort to find the most bottom
	std::sort(vertices.begin(), vertices.end(), [](const Vec3f& a, const Vec3f& b) { return a.y < b.y; });
	Vec3f a = vertices[0];	// most bottom point
	Vec3f b = vertices[1];	// second most bottom point
	Vec3f c = vertices[2];	// top point

	// make the triangle vertices go in CCW order
	Vec3f normal = (c - a).cross(b - a);
	if (normal.z > 0)
		std::swap(b, c);

	// compute triangle area so we can determine the barycentric coordinates, compute uv and sample the texture
	float triangleArea = (t.v2ss - t.v1ss).cross(t.v3ss - t.v1ss).magnitude() * 0.5f;

	// for every line determine the start and end with the same approach as in drawing line
	// and then draw all pixels on the x axis in between.
	// Once you hit the end of one of these lines, continue with the other one connected (the third, remaining)

	Vec3f firstLine = c - a;
	Vec3f secondLine = b - a;

	// this the amount of x pixels added when we advance one row up
	float dxFirst = firstLine.y > 0 ? static_cast<float>(firstLine.x) / static_cast<float>(firstLine.y) : 0;
	float dxSecond = secondLine.y > 0 ? static_cast<float>(secondLine.x) / static_cast<float>(secondLine.y) : 0;

	float dzFirst = firstLine.y > 0 ? static_cast<float>(firstLine.z) / static_cast<float>(firstLine.y) : 0;
	float dzSecond = secondLine.y > 0 ? static_cast<float>(secondLine.z) / static_cast<float>(secondLine.y) : 0;

	const int OUT_OF_BOUNDS_LINE = image.get_height() + 1;
	int topLine = OUT_OF_BOUNDS_LINE;

	// we start from vertex a both and end line but eventually one of these will change depending on
	// if line a->c ends sooner or a->b ends sooner

	Vec3f startPointLeft = a;
	Vec3f startPointRight = a;

	// once we have a as most bottom and vertices in CCW order we always draw from the a->c line to the a->b line
	// then, depending on whether b.y is lower then c.y the a->c line switches to c->b line or a->b line switches to b->c line
	// and the starting points for start or end line has to be adjusted accordingly
	for (float line = a.y; line < topLine; )
	{
		// translate from absolute image space into relative triangle space
		float lineLeftYRelative = line - startPointLeft.y;
		float lineRightYRelative = line - startPointRight.y;

		// determine the start and end of the line
		int xStart = startPointLeft.x + static_cast<int>(dxFirst * static_cast<float>(lineLeftYRelative));
		int xEnd = startPointRight.x + static_cast<int>(dxSecond * static_cast<float>(lineRightYRelative));

		float zStart = startPointLeft.z + static_cast<float>(dzFirst * static_cast<float>(lineLeftYRelative));
		float zEnd = startPointRight.z + static_cast<float>(dzSecond * static_cast<float>(lineRightYRelative));

		//float leftLineT = lineLeftYRelative / leftLine.y;
		//float rightLineT = lineRightYRelative / rightLine.y;


		// draw the whole line
		float xDiff = xEnd - xStart;
		float zDiff = zEnd - zStart;
		for (float x = xStart; x <= xEnd; x++)
		{
			float xT = xDiff != 0 ? (float)(x - xStart) / (float)xDiff : 0.f;
			float zFloat = xT * zDiff + zStart;
			int z = (int)zFloat;
			float z01 = zFloat / (float)farPlaneCoord;

			Vec3i imagePos{ (int)x, (int)line, - (int) (zFloat * farPlaneCoord) };

			// check the bounds
			if (imagePos.x < 0 || imagePos.x >= image.get_width() ||
				imagePos.y < 0 || imagePos.y >= image.get_height())
				continue;

			const bool zTest = zBuffer.TestAndWrite(imagePos);

			TGAColor finalColor = tint;

			if (zTest)
			{
				{ // barycentric coordinates (u,v,w) computation, texture coordinate (r,s) computation and texture sampling

					Vec3f p{ x, line, zFloat };
					float u = ((t.v2ss - p).cross(t.v3ss - p).magnitude() * 0.5f) / triangleArea;
					float v = ((t.v1ss - p).cross(t.v3ss - p).magnitude() * 0.5f) / triangleArea;
					//float w = ((t.v2i - p).cross(t.v1i - p).magnitude() * 0.5f) / triangleArea;
					float w = 1.0f - (u + v); // this is way better, although it should technically be the same as the line above

					fragmentShader.SetBarycentricCoordinates({ u, v, w });
					const bool shouldRender = fragmentShader.fragment();
					if (!shouldRender)
						continue;

					Vec4f fragColor = fragmentShader.GetFinalColor();
					finalColor = TGAColor::FromVec4(fragColor);
				}

				image.set(imagePos.x, imagePos.y, finalColor);
			}
		}

		line++;
		if (topLine == OUT_OF_BOUNDS_LINE)
		{
			if (line > c.y)
			{
				Vec3f thirdLine = b - c;
				float dxThird = static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y);
				float dzThird = static_cast<float>(thirdLine.z) / static_cast<float>(thirdLine.y);

				dxFirst = dxThird;
				dzFirst = dzThird;
				topLine = b.y;
				startPointLeft = c;
			}
			else if (line > b.y)
			{
				Vec3f thirdLine = c - b;
				float dxThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y) : 0;
				float dzThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.z) / static_cast<float>(thirdLine.y) : 0;

				dxSecond = dxThird;
				dzSecond = dzThird;
				topLine = c.y;
				startPointRight = b;
			}
		}
	}
}

/// <summary>
///	Draws the triangle given by 3 coordinates that have Z coordinate as int
/// </summary>
void DrawTriangleMethod3_WithZ(const Triangle& t, TGAImage& image, const TGAColor& color, 
	int farPlaneCoord, ZBufferBase& zBuffer)
{
#if 0
	PROFILE_FUNCTION()
	std::array vertices{ t.v1ss, t.v2ss, t.v2ss};

	// sort to find the most bottom
	std::sort(vertices.begin(), vertices.end(), [](const Vec3i& a, const Vec3i& b) { return a.y < b.y; });
	Vec3i a = vertices[0];	// most bottom point
	Vec3i b = vertices[1];	// second most bottom point
	Vec3i c = vertices[2];	// top point

	// make the triangle vertices go in CCW order
	Vec3i normal = (c - a).cross(b - a);
	if (normal.z > 0)
		std::swap(b, c);

	// for every line determine the start and end with the same approach as in drawing line
	// and then draw all pixels on the x axis in between.
	// Once you hit the end of one of these lines, continue with the other one connected (the third, remaining)

	Vec3i firstLine = c - a;
	Vec3i secondLine = b - a;

	// this the amount of x pixels added when we advance one row up
	float dxFirst = firstLine.y > 0 ? static_cast<float>(firstLine.x) / static_cast<float>(firstLine.y) : 0;
	float dxSecond = secondLine.y > 0 ? static_cast<float>(secondLine.x) / static_cast<float>(secondLine.y) : 0;

	float dzFirst = firstLine.y > 0 ? static_cast<float>(firstLine.z) / static_cast<float>(firstLine.y) : 0;
	float dzSecond = secondLine.y > 0 ? static_cast<float>(secondLine.z) / static_cast<float>(secondLine.y) : 0;

	const int OUT_OF_BOUNDS_LINE = image.get_height() + 1;
	int topLine = OUT_OF_BOUNDS_LINE;

	// we start from vertex a both and end line but eventually one of these will change depending on
	// if line a->c ends sooner or a->b ends sooner

	Vec3i startPointLeft = a;
	Vec3i startPointRight = a;

	// once we have a as most bottom and vertices in CCW order we always draw from the a->c line to the a->b line
	// then, depending on whether b.y is lower then c.y the a->c line switches to c->b line or a->b line switches to b->c line
	// and the starting points for start or end line has to be adjusted accordingly
	for (int line = a.y; line < topLine; )
	{
		// translate from absolute image space into relative triangle space
		int lineLeftYRelative = line - startPointLeft.y;
		int lineRightYRelative = line - startPointRight.y;

		// determine the start and end of the line
		int xStart = startPointLeft.x + static_cast<int>(dxFirst * static_cast<float>(lineLeftYRelative));
		int xEnd = startPointRight.x + static_cast<int>(dxSecond * static_cast<float>(lineRightYRelative));

		int zStart = startPointLeft.z + static_cast<int>(dzFirst * static_cast<float>(lineLeftYRelative));
		int zEnd = startPointRight.z + static_cast<int>(dzSecond * static_cast<float>(lineRightYRelative));

		//float leftLineT = lineLeftYRelative / leftLine.y;
		//float rightLineT = lineRightYRelative / rightLine.y;
		

		// draw the whole line
		int xDiff = xEnd - xStart;
		int zDiff = zEnd - zStart;
		for (int x = xStart; x <= xEnd; x++)
		{
			float xT = xDiff != 0 ? (float)(x - xStart) / (float)xDiff : 0.f;
			float zFloat = xT * zDiff + zStart;
			int z = (int) zFloat;
			float z01 = zFloat / (float) farPlaneCoord;
			const bool zTest = zBuffer.TestAndWrite({ x, line, z });

			if (zTest)
				image.set(x, line, color);
		}

		line++;
		if (topLine == OUT_OF_BOUNDS_LINE)
		{
			if (line > c.y)
			{
				Vec3i thirdLine = b - c;
				float dxThird = static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y);
				float dzThird = static_cast<float>(thirdLine.z) / static_cast<float>(thirdLine.y);

				dxFirst = dxThird;
				dzFirst = dzThird;
				topLine = b.y;
				startPointLeft = c;
			}
			else if (line > b.y)
			{
				Vec3i thirdLine = c - b;
				float dxThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y) : 0;
				float dzThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.z) / static_cast<float>(thirdLine.y) : 0;

				dxSecond = dxThird;
				dzSecond = dzThird;
				topLine = c.y;
				startPointRight = b;
			}
		}
	}
#endif
}

void triangle_reference(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage& image, TGAColor color) {
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
