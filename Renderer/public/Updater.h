#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <d3d12.h>
#include <chrono>

namespace rendering
{
	class Updater : public BaseObject
	{
	public:
		enum UpdaterState
		{
			NotStarted,
			Render,
			Sync,
		};

		class NotifyUpdater : public jobs::Job
		{
		private:
			rendering::Updater& m_updater;
		public:
			NotifyUpdater(rendering::Updater& updater);
			void Do() override;
		};

	private:
		ID3D12CommandList** m_copyLists = nullptr;
		UINT64 m_numCopyLists = 0;

		UpdaterState m_state = NotStarted;
		int m_updatesToWaitFor = 0;

		std::chrono::system_clock::time_point m_lastTick;
		void StartUpdate();

		void RunTickUpdaters(double dt);
		void RunAsyncTickUpdaters(double dt);
		void SyncMutableBuffers();

		double TimeStamp();
	public:
		Updater();
		virtual ~Updater();

		void Proceed();

		void Start();
	};
}