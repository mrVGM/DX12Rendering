#include "CombinatoryEntryPoint.h"

#include "CombinatoryEntryPointMeta.h"

#include "Combinatory.h"

combinatory::CombinatoryEntryPoint::CombinatoryEntryPoint() :
	settings::AppEntryPoint(CombinatoryEntryPointMeta::GetInstance())
{
}

combinatory::CombinatoryEntryPoint::~CombinatoryEntryPoint()
{
}

void combinatory::CombinatoryEntryPoint::Boot()
{
	combinatory::Boot();
}
