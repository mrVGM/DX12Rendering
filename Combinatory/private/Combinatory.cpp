#include "Combinatory.h"

#include "CombinatoryEntryPoint.h"

#include "CombinatorySettings.h"

#include <iostream>

void combinatory::Boot()
{
	std::cout << "Combinatory!" << std::endl;
	new CombinatorySettings();
}

void combinatory::RegisterLib()
{
	new CombinatoryEntryPoint();
}