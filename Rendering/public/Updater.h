#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <chrono>

namespace rendering
{
	class Updater : public BaseObject
	{
	private:
		std::chrono::system_clock::time_point m_lastTick;
		void StartUpdate(double dt);
	public:
		Updater();
		virtual ~Updater();

		int m_updatesToWaitFor = 0;
		void TryStartUpdate();

		void Start();
	};
}