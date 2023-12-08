#pragma once

#include <vector>

class HRTimer
{
public:

	HRTimer(unsigned int numberOfSamples);

	double getFrameTime(void);

	void setMaxDeltaTime(double newMaxDeltaTime);

private:
	HRTimer();

	double m_lastTime;

	std::vector<double> m_vecDeltaTimes;

	unsigned int m_nextBufferIndex = 0;

	static const double MAX_DELTA_TIME_DEFAULT;

	double m_MaxDeltaTime;
};

