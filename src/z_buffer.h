#pragma once

#include <array>
#include "geometry.h"
#include "constants.h"

class ZBufferBase
{
public:
	virtual bool TestAndWrite(const Vec3i& vec) = 0;
	virtual bool Test(const Vec3i& vec) = 0;
};

class ZBuffer2D : public ZBufferBase
{
public:
	bool TestAndWrite(const Vec3i& vec) override { return true; }
	bool Test(const Vec3i& vec) override { return true; }
};

template
<typename T, int width, int height, T invalid_value>
class ZBuffer : public ZBufferBase
{
public:
	ZBuffer()
	{
		for (T& val : m_Buffer)
			val = invalid_value;
	}

	// tests the point against the zbuffer value and write if the z value is lower
	bool TestAndWrite(const Vec3i& vec) override;

	// tests the point agains the z buffer and returns whether the z value is lower (true) or not (false)
	bool Test(const Vec3i& vec) override;

private:
	std::array<T, width * height> m_Buffer;
};

template <typename T, int width, int height,T invalid_value >
bool ZBuffer<T, width, height, invalid_value>::TestAndWrite(const Vec3i& vec)
{
	int index = vec.y * (width - 1) + vec.x;
	T val = m_Buffer[index];
	const bool testResult = (vec.z <= val);
	//printf("Testing at (%d, %d, %d): %s, stored: %d \n", vec.x, vec.y, vec.z, testResult ? "false" : "true", m_Buffer[index]);

	if (testResult)
		return false;

	m_Buffer[index] = vec.z;

	return true;
}

template <typename T, int width, int height,T invalid_value >
bool ZBuffer<T, width, height, invalid_value >::Test(const Vec3i& vec)
{
	T val = m_Buffer[vec.y * width + vec.x];
	return vec.z > val;
}

using ZBufferIntDefault = ZBuffer<int, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, std::numeric_limits<int>::min()>;
