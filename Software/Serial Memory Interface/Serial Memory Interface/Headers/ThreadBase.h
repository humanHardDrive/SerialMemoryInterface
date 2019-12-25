#pragma once

#include <thread>
#include <atomic>
#include <utility>

#include "LockingQ.h"

class ThreadBase
{
public:
	ThreadBase();
	~ThreadBase();

	void start();
	void stop();

	bool isRunning() { return m_bIsRunning; }

protected:
	virtual void processing() = 0;

private:
	std::thread m_ProcessingThread;
	std::atomic_bool m_bIsRunning;
};