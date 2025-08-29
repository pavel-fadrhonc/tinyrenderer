#pragma once

#include <chrono>

namespace sor
{
	// local to translation unit
	static long long GetStartupTime();
	
	inline float GetTimeSinceStartupMiliseconds();

	static inline long long s_StartupTimeMicroseconds = GetStartupTime();

	static long long GetStartupTime()
	{
		auto now = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
	}

    inline float GetTimeSinceStartupMiliseconds()
    {
		// Get the current time as a time_point
		auto now = std::chrono::system_clock::now();

		const long long currentTimeMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		const long long timeSinceStartup = currentTimeMicroseconds - s_StartupTimeMicroseconds;

		return static_cast<float>(timeSinceStartup) / 1000.f;
	}

	inline float GetTimeSinceStartupSeconds()
	{
		return GetTimeSinceStartupMiliseconds() / 1000.f;
	}

	class Time
	{
		friend void RunLoop();
		friend float GetDeltaTime();
	private:
		float m_fDeltaTime;
	};

	inline Time g_Time;

	inline float GetDeltaTime()
	{
		return g_Time.m_fDeltaTime;
	}
}