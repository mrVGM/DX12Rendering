#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include "SceneConverterSettings.h"

#include <iostream>

void scene_converter::Boot()
{
	std::cout << "Scene Converter!" << std::endl;

	new SceneConverterSettings();
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}