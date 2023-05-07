#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <chrono>

namespace rendering
{
	class NotifyUpdater : public jobs::Job
	{
		void Do() override;
	};

	class Updater : public BaseObject
	{
		friend class NotifyUpdater;
	private:
		int m_updatesToWaitFor = 0;

		std::chrono::system_clock::time_point m_lastTick;
		void StartUpdate(double dt);
		void TryStartUpdate();
	public:
		Updater();
		virtual ~Updater();

		void Start();
	};
}