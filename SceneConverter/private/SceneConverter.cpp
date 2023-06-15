#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include <iostream>

void scene_converter::Boot()
{
	std::cout << "Scene Converter!" << std::endl;
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}