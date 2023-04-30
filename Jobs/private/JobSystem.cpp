#include "JobSystem.h"

#include "JobSystemMeta.h"
#include "Thread.h"

jobs::JobSystem::JobSystem(const BaseObjectMeta& meta, int numThreads) :
	BaseObject(meta),
	m_stateNotify(*this),
	m_numThreads(numThreads)
{	
	for (int i = 0; i < m_numThreads; ++i)
	{
		m_threads.push_back(new Thread(m_stateNotify));
	}

	for (int i = 0; i < m_numThreads; ++i)
	{
		m_threads[i]->Start();
	}
}

jobs::JobSystem::~JobSystem()
{
	while (!m_jobsToDo.empty())
	{
		Job* job = m_jobsToDo.front();
		delete job;
		m_jobsToDo.pop();
	}

	for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
	{
		delete *it;
	}
}

jobs::ThreadStateNotify::ThreadStateNotify(JobSystem& jobSystem) :
	m_jobSystem(jobSystem)
{
}

void jobs::ThreadStateNotify::HasBecomeBusy(jobs::Thread& thread)
{
	m_jobSystem.IncrementFreeThreadsCounter(-1);
}

void jobs::ThreadStateNotify::HasBecomeFree(jobs::Thread& thread)
{
	m_jobSystem.IncrementFreeThreadsCounter(1);
}

void jobs::JobSystem::IncrementFreeThreadsCounter(int increment)
{
	m_threadScheduleMutex.lock();
	m_freeThreads += increment;
	m_threadScheduleMutex.unlock();

	if (m_freeThreads > 0)
	{
		TryStartJob();
	}
}

void jobs::JobSystem::ScheduleJob(Job* job)
{
	m_threadScheduleMutex.lock();
	m_jobsToDo.push(job);
	m_threadScheduleMutex.unlock();

	TryStartJob();
}

void jobs::JobSystem::TryStartJobSafe()
{
	m_threadScheduleMutex.lock();
	TryStartJob();
	m_threadScheduleMutex.unlock();
}

void jobs::JobSystem::TryStartJob()
{
	if (m_freeThreads == 0)
	{
		return;
	}
	if (m_jobsToDo.empty())
	{
		return;
	}

	Thread* thread = nullptr;

	for (auto it = m_threads.begin(); it != m_threads.end(); ++it)
	{
		Thread* tmp = *it;
		if (tmp->IsFree())
		{
			thread = tmp;
			break;
		}
	}

	if (!thread)
	{
		return;
	}
	Job* job = m_jobsToDo.front();
	m_jobsToDo.pop();

	thread->SetJob(*job);
	thread->GetSemaphore().release();
}
