#include "Thread.h"

#include "Job.h"
#include "JobSystem.h"

#include <mutex>

#include <iostream>

namespace
{
	std::mutex m_startThreadMutex;

	std::binary_semaphore m_curThreadSemaphore{ 1 };
	jobs::Thread* m_curThread = nullptr;

	void run()
	{
		jobs::Thread& myThread = *m_curThread;
		m_curThread = nullptr;
		m_curThreadSemaphore.release();

		std::binary_semaphore& semaphore = myThread.GetSemaphore();

		myThread.SetIsFree(true);

		while (true)
		{
			semaphore.acquire();
			if (myThread.IsStopped())
			{
				semaphore.release();
				return;
			}

			myThread.SetIsFree(false);

			jobs::Job* job = myThread.AcquireJob();

			job->Do();
			delete job;

			myThread.SetIsFree(true);
		}
	}
}

jobs::Thread::Thread(ThreadStateNotify& threadStateNotify) :
	m_stateNotify(threadStateNotify)
{
}

void jobs::Thread::Start()
{
	m_startThreadMutex.lock();

	m_semaphore = new std::binary_semaphore{ 1 };
	m_semaphore->acquire();

	m_curThreadSemaphore.acquire();
	m_curThread = this;
	m_thread = new std::thread(run);

	m_curThreadSemaphore.acquire();
	m_curThreadSemaphore.release();

	m_startThreadMutex.unlock();
}

std::binary_semaphore& jobs::Thread::GetSemaphore()
{
	return *m_semaphore;
}

bool jobs::Thread::IsStopped() const
{
	return m_stopped;
}

bool jobs::Thread::IsFree() const
{
	return m_free;
}

void jobs::Thread::SetIsFree(bool free)
{
	m_free = free;
	if (m_free)
	{
		m_stateNotify.HasBecomeFree(*this);
	}
	else
	{
		m_stateNotify.HasBecomeBusy(*this);
	}
}

void jobs::Thread::SetJob(Job& job)
{
	m_curJob = &job;
	m_semaphore->release();
}

jobs::Job* jobs::Thread::AcquireJob()
{
	jobs::Job* tmp = m_curJob;
	m_curJob = nullptr;
	return tmp;
}

void jobs::Thread::Stop()
{
	m_stopped = true;
	m_semaphore->release();
}

jobs::Thread::~Thread()
{
	Stop();
	m_thread->join();

	if (m_curJob)
	{
		delete m_curJob;
	}
	m_curJob = nullptr;

	delete m_semaphore;
	delete m_thread;
}

