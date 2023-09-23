#include "SceneConverterEntryPoint.h"

#include "SceneConverterEntryPointMeta.h"

#include "SceneConverter.h"

scene_converter::SceneConverterEntryPoint::SceneConverterEntryPoint() :
	settings::AppEntryPoint(SceneConverterEntryPointMeta::GetInstance())
{
}

scene_converter::SceneConverterEntryPoint::~SceneConverterEntryPoint()
{
}

void scene_converter::SceneConverterEntryPoint::Boot()
{
	scene_converter::Boot();
}

void scene_converter::SceneConverterEntryPoint::Shutdown()
{
}
