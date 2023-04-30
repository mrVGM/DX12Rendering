#pragma once

#include <thread>
#include <semaphore>
#include <mutex>

namespace jobs
{
	class Job;
	class ThreadStateNotify;

	class Thread
	{
	private:
		std::binary_semaphore* m_semaphore = nullptr;
		std::thread* m_thread = nullptr;
		bool m_free = true;
		bool m_stopped = false;
		Job* m_curJob = nullptr;
	public:
		ThreadStateNotify& m_stateNotify;

		Thread(ThreadStateNotify& notify);
		void Start();
		std::binary_semaphore& GetSemaphore();
		bool IsStopped() const;
		bool IsFree() const;
		void SetIsFree(bool free);

		void SetJob(Job& job);
		Job* AcquireJob();
		
		void Stop();

		~Thread();
	};
}