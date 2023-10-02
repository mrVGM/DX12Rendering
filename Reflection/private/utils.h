#pragma once

#include <string>

#include "Job.h"

namespace reflection
{
	std::string GetNewId();
	void RunMain(jobs::Job* job);
}