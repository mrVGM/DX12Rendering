#include "Combinatory.h"

#include "CombinatoryEntryPoint.h"

#include <iostream>

void combinatory::Boot()
{
	std::cout << "Combinatory!" << std::endl;
}

void combinatory::RegisterLib()
{
	new CombinatoryEntryPoint();
}