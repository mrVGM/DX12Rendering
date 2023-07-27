#include "Combinatory.h"

#include "CombinatoryEntryPoint.h"

#include "CombinatorySettings.h"

#include "JobSystem.h"
#include "ProcessorsJobSystemMeta.h"
#include "ResultJobSystemMeta.h"
#include "LogJobSystemMeta.h"

#include <iostream>

void combinatory::Boot()
{
	std::cout << "Combinatory!" << std::endl;
	CombinatorySettings* settings = new CombinatorySettings();

	new jobs::JobSystem(ProcessorsJobSystemMeta::GetInstance(), settings->GetSettings().m_numThreads);
	new jobs::JobSystem(ResultJobSystemMeta::GetInstance(), 1);
	new jobs::JobSystem(LogJobSystemMeta::GetInstance(), 1);
}

void combinatory::RegisterLib()
{
	new CombinatoryEntryPoint();
}