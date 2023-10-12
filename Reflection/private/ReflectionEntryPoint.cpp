#include "ReflectionEntryPoint.h"

#include "ReflectionEntryPointMeta.h"

#include "Reflection.h"

reflection::ReflectionEntryPoint::ReflectionEntryPoint() :
	settings::AppEntryPoint(ReflectionEntryPointMeta::GetInstance())
{
}

reflection::ReflectionEntryPoint::~ReflectionEntryPoint()
{
}

void reflection::ReflectionEntryPoint::Boot()
{
	reflection::Boot();
}

void reflection::ReflectionEntryPoint::Shutdown()
{
}
