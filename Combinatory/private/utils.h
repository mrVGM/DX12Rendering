#pragma once

#include "CombinatorySettings.h"

#include "Job.h"

namespace combinatory
{
	CombinatorySettings* GetSettings();

	int GCD(int a, int b);
	int LCM(int a, int b);

	void RunAsync(jobs::Job* job);
	void RunSync(jobs::Job* job);
}