#include "HRTimer.h"

#include "../Core/OpenGlCommons.h"

//static 
const double HRTimer::MAX_DELTA_TIME_DEFAULT = 1.0 / 30.0;

HRTimer::HRTimer(unsigned int numberOfSamples) {

    this->m_lastTime = glfwGetTime();
    this->m_MaxDeltaTime = HRTimer::MAX_DELTA_TIME_DEFAULT;

    for (int count = 0; count != numberOfSamples; count++) {
        this->m_vecDeltaTimes.push_back(HRTimer::MAX_DELTA_TIME_DEFAULT);
    }
}

void HRTimer::setMaxDeltaTime(double newMaxDeltaTime) {
    this->m_MaxDeltaTime = newMaxDeltaTime;
    return;
}

double HRTimer::getFrameTime(void)
{
    // Get the delta since last time we checked
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - m_lastTime;

    if (deltaTime > HRTimer::MAX_DELTA_TIME_DEFAULT) {
        deltaTime = HRTimer::MAX_DELTA_TIME_DEFAULT;
    }

    this->m_lastTime = currentTime;
    this->m_vecDeltaTimes[this->m_nextBufferIndex] = deltaTime;
    this->m_nextBufferIndex++;

    // Is the index out of bounds?
    if (this->m_nextBufferIndex >= (unsigned int)this->m_vecDeltaTimes.size()) {
        this->m_nextBufferIndex = 0;
    }

    // Get the average
    double totalDeltaTime = 0.0;
    for (unsigned int index = 0; index != this->m_vecDeltaTimes.size(); index++) {
        totalDeltaTime += this->m_vecDeltaTimes[index];
    }

    double averageDelta = totalDeltaTime / (double)this->m_vecDeltaTimes.size();
    return averageDelta;
}