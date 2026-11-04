#include "TimeManager.h"

void TimeManager::Init()
{
	m_remainingTime = kTimeLimit;

	m_isRunning = false;
	m_isTimeUp = false;
}

void TimeManager::Start()
{
	if (m_isTimeUp)
	{
		return;
	}

	m_isRunning = true;
}

void TimeManager::Stop()
{
	m_isRunning = false;
}

void TimeManager::Update(float deltaTime)
{
	if (!m_isRunning || m_isTimeUp)
	{
		return;
	}

	m_remainingTime -= deltaTime;

	if (m_remainingTime <= 0.0f)
	{
		m_remainingTime = 0.0f;

		m_isTimeUp = true;
		m_isRunning = false;
	}
}