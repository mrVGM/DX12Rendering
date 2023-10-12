#include "ReflectionEntryPoint.h"

#include "ReflectionEntryPointMeta.h"

reflection::ReflectionEntryPoint::ReflectionEntryPoint() :
	settings::AppEntryPoint(ReflectionEntryPointMeta::GetInstance())
{
}

reflection::ReflectionEntryPoint::~ReflectionEntryPoint()
{
}

void reflection::ReflectionEntryPoint::Boot()
{
	bool t = true;
}

void reflection::ReflectionEntryPoint::Shutdown()
{
}
