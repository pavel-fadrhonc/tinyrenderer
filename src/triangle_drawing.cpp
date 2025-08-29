#include "triangle_drawing.h"

#include "triangle_drawing_test.h"

namespace sor
{
	void DrawTriangleWired(const Triangle& t, Texture& output, const TGAColor& color)
	{
				
		std::array perspDivVerts{ t.v0ss.FromHomogeneous(), t.v1ss.FromHomogeneous(), t.v2ss.FromHomogeneous()};
		// std::array vertsScreenSpace{
		// 	Vec3f{ perspDivVerts[0].x, perspDivVerts[0].y, 1.f},
		// 	Vec3f{ perspDivVerts[1].x, perspDivVerts[1].y, 1.f},
		// 	Vec3f{ perspDivVerts[2].x, perspDivVerts[2].y, 1.f},
		// };

		DrawLine(perspDivVerts[0].x, perspDivVerts[0].y, perspDivVerts[1].x, perspDivVerts[1].y, output, color);
		DrawLine(perspDivVerts[1].x, perspDivVerts[1].y, perspDivVerts[2].x, perspDivVerts[2].y, output, color);
		DrawLine(perspDivVerts[0].x, perspDivVerts[0].y, perspDivVerts[2].x, perspDivVerts[2].y, output, color);
	}

	void DrawTriangleMethod1(const Vec3i& v1, const Vec3i& v2, const Vec3i& v3, Texture& texture, const TGAColor& color)
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

		const Vec2i imageDimensions{ texture.GetWidth(), texture.GetHeight() };

		//std::array vertices{
		//	ModelSpaceToImageSpace(v1, imageDimensions),
		//	ModelSpaceToImageSpace(v2, imageDimensions),
		//	ModelSpaceToImageSpace(v3, imageDimensions)};

		std::array vertices{ v1, v2,	v3 };

		std::sort(vertices.begin(), vertices.end(), [](const Vec3i& a, const Vec3i& b) { return a.y < b.y; });
		Vec3i& b1 = vertices[0];	// most bottom point
		Vec3i& b2 = vertices[1];	// second most bottom point
		Vec3i& b3 = vertices[2];	// top point

		auto DrawLineFromBottomSide = [&b3](int x, int y, Texture& texture, TGAColor color)
		{
			DrawLine(x, y, b3.x, b3.y, texture , color);
		};

		ForEachLinePixel(b1.x, b1.y, b2.x, b2.y, texture, color, DrawLineFromBottomSide);
	}

	void DrawTriangleMethod2(const Vec3i& v1_, const Vec3i& v2_, const Vec3i& v3_, TGAImage& image,
		const TGAColor& color)
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

	void DrawTriangleMethod3_2DCoords(const Vec3i& v1_, const Vec3i& v2_, const Vec3i& v3_, TGAImage& image,
	                                  const TGAColor& color)
	{
		PROFILE_FUNCTION();
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


	void DrawTriangle_Standard(const Triangle& t, Texture& outputTex, ZBufferBase& zBuffer, IFragmentShader& fragmentShader)
	{
		// find a bounding box for the triangle
		std::array perspDivVerts{ t.v0ss.FromHomogeneous(), t.v1ss.FromHomogeneous(), t.v2ss.FromHomogeneous()};
		std::array vertsScreenSpace{
			Vec3f{ perspDivVerts[0].x, perspDivVerts[0].y, 1.f},
			Vec3f{ perspDivVerts[1].x, perspDivVerts[1].y, 1.f},
			Vec3f{ perspDivVerts[2].x, perspDivVerts[2].y, 1.f},
		};

		Vec2i bbox_min{ (int)std::min(perspDivVerts[0].x, std::min(perspDivVerts[1].x, perspDivVerts[2].x)),
			(int)std::min(perspDivVerts[0].y, std::min(perspDivVerts[1].y, perspDivVerts[2].y)) };
		Vec2i bbox_max{ (int) std::max(perspDivVerts[0].x, std::max( perspDivVerts[1].x, perspDivVerts[2].x)),
			(int) std::max(perspDivVerts[0].y, std::max(perspDivVerts[1].y, perspDivVerts[2].y)) };

		Vec3u64 a = { (u64) vertsScreenSpace[0].x, (u64) vertsScreenSpace[0].y, (u64) vertsScreenSpace[0].z};
		Vec3u64 b = { (u64) vertsScreenSpace[1].x, (u64) vertsScreenSpace[1].y, (u64) vertsScreenSpace[1].z};
		Vec3u64 c = { (u64) vertsScreenSpace[2].x, (u64) vertsScreenSpace[2].y, (u64) vertsScreenSpace[2].z};

		Vec3u64 aTOb = (b - a);
		Vec3u64 aTOc = c - a;
		Vec3u64 crossProduct = aTOb.cross(aTOc);
		u64 sqrtMag = ((u64)crossProduct.x * (u64)crossProduct.x + (u64)crossProduct.y * (u64)crossProduct.y + (u64)crossProduct.z * (u64)crossProduct.z);
		double triangleArea = std::sqrt(sqrtMag) * 0.5;

		// make the triangle perspDivVerts go in CCW order
		Vec3u64 normal = (c - a).cross(b - a);
		if (normal.z > 0)
			std::swap(b, c);

		for (int y = bbox_min.y; y < bbox_max.y; y++)
		{
			for (int x = bbox_min.x; x < bbox_max.x; x++)
			{
				const Vec3u64 p{ (u64)x, (u64)y, 1 };

				float alpha =(((b - p).cross(c - p).magnitude() * 0.5) / triangleArea);
				float beta = ((a - p).cross(c - p).magnitude() * 0.5 / triangleArea);
				float gamma =((a - p).cross(b - p).magnitude() * 0.5 / triangleArea);
				// float gamma = 1.0f - (alpha + beta);

				if (alpha < 0.f || beta < 0.f || gamma < 0.f)
				{
					printf("Somethings wrong");
				}

				if (alpha >= 0 && beta >= 0 && gamma >= 0)
				{ // inside of triangle
					const Texture& albedoTex = g_DrawContext.albedoTexture.GetTexture();
					Vec2f uv0 = g_DrawContext.model.UVForFaceAndVertex(t.index, 0);
					Vec2f uv1 = g_DrawContext.model.UVForFaceAndVertex(t.index, 1);
					Vec2f uv2 = g_DrawContext.model.UVForFaceAndVertex(t.index, 2);

					if (normal.z > 0)
						std::swap(uv1, uv2);

					const Vec2f interpolatedUV = uv0 * alpha + uv1 * beta + uv2 * gamma;

					const TGAColor textureColor =  albedoTex.GetPixel((int) (interpolatedUV.x * (float) albedoTex.GetWidth()),
						(int) (interpolatedUV.y * (float) albedoTex.GetHeight()));

					const TGAColor uvDebugColor = TGAColor::FromFloat(interpolatedUV.x, interpolatedUV.y, 0.f, 1.f);

					const TGAColor baryDebugColor = TGAColor::FromFloat(alpha, beta, 0.f, 1.f);

					outputTex.SetPixel(x, y, uvDebugColor);
					// outputTex.SetPixel(x, y, baryDebugColor);
				}


			}
		}
	}

	void DrawTriangleMethod3_WithZ_WithTexture(const Triangle& t, Texture& texture, const TGAColor& tint,
		int farPlaneCoord, ZBufferBase& zBuffer, IFragmentShader& fragmentShader)
	{
#define USE_INTS_FOR_TEXTURING 1

		PROFILE_FUNCTION()
		std::array perspDivVerts{ t.v0ss.FromHomogeneous(), t.v1ss.FromHomogeneous(), t.v2ss.FromHomogeneous()};
		std::array vertsScreenSpace{
			Vec3f{ perspDivVerts[0].x, perspDivVerts[0].y, 1.f},
			Vec3f{ perspDivVerts[1].x, perspDivVerts[1].y, 1.f},
			Vec3f{ perspDivVerts[2].x, perspDivVerts[2].y, 1.f},
		};

		// sort to find the most bottom
		std::sort(vertsScreenSpace.begin(), vertsScreenSpace.end(), [](const Vec3f& a, const Vec3f& b) { return a.y < b.y; });
		Vec3f a = vertsScreenSpace[0];	// most bottom point
		Vec3f b = vertsScreenSpace[1];	// second most bottom point
		Vec3f c = vertsScreenSpace[2];	// top point

		// make the triangle perspDivVerts go in CCW order
		Vec3f normal = (c - a).cross(b - a);
		if (normal.z > 0)
			std::swap(b, c);

		// compute triangle area so we can determine the barycentric coordinates, compute uv and sample the texture
		float triangleArea = (vertsScreenSpace[1] - vertsScreenSpace[0]).cross(vertsScreenSpace[2] - vertsScreenSpace[0]).magnitude() * 0.5f;

		// for every line determine the start and end with the same approach as in drawing line
		// and then draw all pixels on the x axis in between.
		// Once you hit the end of one of these lines, continue with the other one connected (the third, remaining)

		Vec3f firstLine = c - a;
		Vec3f secondLine = b - a;

		// this the amount of x pixels added when we advance one row up
		float dxFirst = firstLine.y > 0 ? static_cast<float>(firstLine.x) / static_cast<float>(firstLine.y) : 0;
		float dxSecond = secondLine.y > 0 ? static_cast<float>(secondLine.x) / static_cast<float>(secondLine.y) : 0;

		const int OUT_OF_BOUNDS_LINE = texture.GetHeight() + 1;
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


			// draw the whole line
			float xDiff = xEnd - xStart;
			for (float x = xStart; x <= xEnd; x++)
			{
				float xT = xDiff != 0 ? (float)(x - xStart) / (float)xDiff : 0.f;

				Vec2i imagePos{ (int)x, (int)line };

				// check the bounds
				if (imagePos.x < 0 || imagePos.x >= texture.GetWidth() ||
					imagePos.y < 0 || imagePos.y >= texture.GetHeight())
					continue;


				// texture.SetPixel(imagePos.x, imagePos.y, tint);
				// continue;
				// the barycentric coordinates are coming from perspectively divided position
				// therefore we cannot use them to interpolate vertex attributes
				// instead we need to perspectively divide each attribute, then interpolate them and then "undivide" them
				// by multiplying by 1/w that has also been interpolated
				// interpolate perspectively divided z the same way we're interpolating fragment shader attributes except we don't need to undivide it


				Vec3f p{ x, line, 1.0 };
				float alpha = (b - p).cross(c - p).magnitude() * 0.5f / triangleArea;
				float beta = (a - p).cross(c - p).magnitude() * 0.5f / triangleArea;
				float gamma = 1.0f - (alpha + beta);

				Vec3f barycentricCoordinates{ alpha, beta, gamma };
				
				float fragDepth = Vec3f{perspDivVerts[0].z, perspDivVerts[1].z, perspDivVerts[2].z}.dot(barycentricCoordinates);
				const bool zTest = zBuffer.TestAndWrite((int) p.x, (int) p.y, fragDepth);

				TGAColor finalColor = tint;

				if (true)
				{
					{ // barycentric coordinates (u,v,w) computation, texture coordinate (r,s) computation and texture sampling
						float oneOverW_interpolated = Vec3f{ 1.f / t.v0ss.w(), 1.f / t.v1ss.w(), 1.f / t.v2ss.w() }.dot(barycentricCoordinates);

						fragmentShader.SetInterpolationData( {.barycentricCoordinates= barycentricCoordinates, .verticesW=
							Vec3f{t.v0ss.w(), t.v1ss.w(), t.v2ss.w() }, .interpolatedOneOverW= oneOverW_interpolated});
						const bool shouldRender = fragmentShader.fragment();
						if (!shouldRender)
							continue;

						Vec4f fragColor = fragmentShader.GetFinalColor();
						finalColor = TGAColor::FromVec4(fragColor);
					}

					texture.SetPixel(imagePos.x, imagePos.y, finalColor);
				}
			}

			line++;
			if (topLine == OUT_OF_BOUNDS_LINE)
			{
				if (line > c.y)
				{
					Vec3f thirdLine = b - c;
					float dxThird = static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y);

					dxFirst = dxThird;
					topLine = b.y;
					startPointLeft = c;
				}
				else if (line > b.y)
				{
					Vec3f thirdLine = c - b;
					float dxThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.x) / static_cast<float>(thirdLine.y) : 0;
					float dzThird = thirdLine.y > 0 ? static_cast<float>(thirdLine.z) / static_cast<float>(thirdLine.y) : 0;

					dxSecond = dxThird;
					topLine = c.y;
					startPointRight = b;
				}
			}
		}
	}

	void DrawTriangleMethod3_WithZ(const Triangle& t, TGAImage& image, const TGAColor& color, int farPlaneCoord,
		ZBufferBase& zBuffer)
	{
#if 0
		PROFILE_FUNCTION()
			std::array vertices{ t.v0ss, t.v1ss, t.v1ss };

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
				int z = (int)zFloat;
				float z01 = zFloat / (float)farPlaneCoord;
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
}
