#include "CombinatoryEntryPoint.h"

#include "CombinatoryEntryPointMeta.h"

#include "Combinatory.h"

#include "utils.h"

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

	int gcd = combinatory::GCD(240, 320);
	bool t = true;
}
