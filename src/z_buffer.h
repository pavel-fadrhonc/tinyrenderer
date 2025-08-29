#pragma once

#include <array>
#include <limits>
#include "geometry.h"
#include "constants.h"

namespace sor
{
	//-----------------------------------------------------------------------------------------------------------------
	class ZBufferBase
	{
	public:
		virtual bool TestAndWrite(int x, int y, float depth) = 0;
		virtual bool Test(const Vec3i& vec) = 0;
		virtual void Clear() = 0;

		virtual ~ZBufferBase() = default;
	};

	//-----------------------------------------------------------------------------------------------------------------
	class ZBufferDummy : public ZBufferBase
	{
	public:
		bool TestAndWrite(int x, int y, float depth) override { return true; }
		bool Test(const Vec3i& vec) override { return true; }
		void Clear() override {}
	};

	//-----------------------------------------------------------------------------------------------------------------
	// Generic class Zbuffer implementation
	template <typename T, int width, int height, T invalid_value>
	class ZBuffer : public ZBufferBase
	{
	public:
		ZBuffer()
		{
			for (T& val : m_Buffer)
				val = invalid_value;
		}

		// tests the point against the zbuffer value and write if the z value is lower
		bool TestAndWrite(int x, int y, float depth) override;

		// tests the point agains the z buffer and returns whether the z value is lower (true) or not (false)
		bool Test(const Vec3i& vec) override;

		void Clear() override
		{
			for (T& val : m_Buffer)
				val = invalid_value;
		}

	private:
		std::array<T, width * height> m_Buffer;
	};

	template <typename T, int width, int height, T invalid_value >
	bool ZBuffer<T, width, height, invalid_value>::TestAndWrite(int x, int y, float depth)
	{
		int index = y * (width - 1) + x;
		T val = m_Buffer[index];
		const bool testResult = (depth <= val);
		//printf("Testing at (%d, %d, %d): %s, stored: %d \n", vec.x, vec.y, vec.z, testResult ? "false" : "true", m_Buffer[index]);

		if (testResult)
			return false;

		m_Buffer[index] = depth;

		return true;
	}

	template <typename T, int width, int height, T invalid_value >
	bool ZBuffer<T, width, height, invalid_value >::Test(const Vec3i& vec)
	{
		T val = m_Buffer[vec.y * width + vec.x];
		return vec.z > val;
	}

	//using ZBufferIntDefault = ZBuffer<int, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, std::numeric_limits<int>::min()>;
		//typedef ZBuffer<int, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, std::numeric_limits<int>::min()> ZBufferIntDefault;

	// for some reason it has problems with the construct with std::numeric_limits -- uncomment the code to see it
	class ZBufferFloatDefault : public ZBuffer<float, IMAGE_SIZE_DEFAULT_X, IMAGE_SIZE_DEFAULT_Y, -1000.f>
	{
		
	};


}
