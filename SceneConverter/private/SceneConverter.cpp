#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include "SceneConverterSettings.h"

#include "DataLib.h"
#include "ColladaScene.h"

#include <iostream>

void scene_converter::Boot()
{
	std::cout << "Scene Converter!" << std::endl;

	SceneConverterSettings* settings = new SceneConverterSettings();

	SceneConverterSettings::Settings& sceneSettings = settings->GetSettings();

	std::string fileName = data::GetLibrary().GetRootDir() + "asd.bin";
	

	for (auto it = sceneSettings.m_scenes.begin(); it != sceneSettings.m_scenes.end(); ++it)
	{
		collada::ColladaScene cs;
		cs.Load(data::GetLibrary().GetRootDir() + it->second.m_dae);

		collada::Geometry& refGeo = cs.GetScene().m_geometries.begin()->second;
		{
			data::BinWriter file(fileName);
			refGeo.Serialize(file);
		}

		{
			data::BinReader file(fileName);
			collada::Geometry geo;
			geo.Deserialize(file);
		}
	}
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}