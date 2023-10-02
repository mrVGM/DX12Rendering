#pragma once

#include "Job.h"
#include "ReflectionSettings.h"

#include <string>

namespace reflection
{
	std::string GetNewId();
	reflection::ReflectionSettings* GetReflectionSettings();
	void RunMain(jobs::Job* job);
}