#include "ThreadBase.h"

ThreadBase::ThreadBase() :
	m_bIsRunning(false)
{
}

ThreadBase::~ThreadBase()
{
	stop();

	while (!m_MessageQ.empty())
	{
		delete(m_MessageQ.front().second);
		m_MessageQ.pop();
	}
}

void ThreadBase::start()
{
	if (!m_bIsRunning)
	{
		m_bIsRunning = true;
		m_ProcessingThread = std::thread(&ThreadBase::processing, this);
	}
}

void ThreadBase::stop()
{
	if (m_bIsRunning)
	{
		m_bIsRunning = false;
		if (m_ProcessingThread.joinable())
			m_ProcessingThread.join();
	}
}
