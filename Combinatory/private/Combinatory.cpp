#include "Combinatory.h"

#include "CombinatoryEntryPoint.h"

#include "CombinatorySettings.h"

#include "JobSystem.h"
#include "ProcessorsJobSystemMeta.h"
#include "ResultJobSystemMeta.h"

#include <iostream>

void combinatory::Boot()
{
	std::cout << "Combinatory!" << std::endl;
	new CombinatorySettings();

	new jobs::JobSystem(ProcessorsJobSystemMeta::GetInstance(), 64);
	new jobs::JobSystem(ResultJobSystemMeta::GetInstance(), 1);
}

void combinatory::RegisterLib()
{
	new CombinatoryEntryPoint();
}