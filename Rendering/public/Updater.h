#pragma once

#include "BaseObject.h"

#include "Job.h"

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

	private:
		UpdaterState m_state = NotStarted;

		std::chrono::system_clock::time_point m_lastTick;
		void StartUpdate();

		void RunTickUpdaters(double dt);
		void RunRDUs();

		double TimeStamp();
	public:
		Updater();
		virtual ~Updater();

		int m_updatesToWaitFor = 0;
		void Proceed();

		void Start();
	};
}