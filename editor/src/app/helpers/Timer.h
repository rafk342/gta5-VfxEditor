#pragma once

#include <chrono>
#include "common/types.h"


class Timer
{
	using TClock = std::chrono::high_resolution_clock;
	using TTime = std::chrono::time_point<std::chrono::high_resolution_clock>;

	TTime m_StartTime;
	TTime m_EndTime;

	bool m_IsRunning = false;
public:
	Timer()
	{
		Reset();
	}

	static Timer StartNew()
	{
		Timer result;
		result.Start();
		return result;
	}

	void Start()
	{
		m_StartTime = TClock::now();
		m_IsRunning = true;
	}

	void Stop()
	{
		if (!m_IsRunning)
			return;

		m_EndTime = TClock::now();
		m_IsRunning = false;
	}

	void Restart()
	{
		Reset();
		Start();
	}

	void Reset()
	{
		TTime time = TClock::now();
		m_StartTime = time;
		m_EndTime = time;
	}

	bool GetIsRunning() const { return m_IsRunning; }

	u64 GetElapsedTicks() const
	{
		return (m_EndTime - m_StartTime).count();
	}

	double GetElapsedSeconds() const
	{
		double duration = std::chrono::duration_cast<std::chrono::duration<double>>(m_EndTime - m_StartTime).count();
		return duration;
	}

	u64 GetElapsedMilliseconds() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_EndTime - m_StartTime).count();
	}

	u64 GetElapsedMicroseconds() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(m_EndTime - m_StartTime).count();
	}
};
