#pragma once

#include "BaseObject.h"

#include <queue>
#include <vector>

#include <mutex>

namespace jobs
{
	class Job;
	class Thread;
	class JobSystem;

	class ThreadStateNotify
	{
	private:
		JobSystem& m_jobSystem;
	public:
		ThreadStateNotify(JobSystem& jobSystem);
		void HasBecomeBusy(Thread& thread);
		void HasBecomeFree(Thread& thread);
	};

	class JobSystem : public BaseObject
	{
		friend class ThreadStateNotify;
	private:
		std::mutex m_threadScheduleMutex;
		const int m_numThreads = -1;
		std::queue<Job*> m_jobsToDo;
		int m_freeThreads = 0;

		ThreadStateNotify m_stateNotify;
		std::vector<Thread*> m_threads;

		void IncrementFreeThreadsCounter(int increment);		
		void TryStartJobSafe();
		void TryStartJob();
	public:
		JobSystem(const BaseObjectMeta& meta, int numThreads);
		void ScheduleJob(Job* job);
		virtual ~JobSystem();
	};
}