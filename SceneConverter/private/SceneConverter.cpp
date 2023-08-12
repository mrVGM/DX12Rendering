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

	data::MemoryFile mf;
	for (auto it = sceneSettings.m_scenes.begin(); it != sceneSettings.m_scenes.end(); ++it)
	{
		collada::ColladaScene cs;
		cs.Load(data::GetLibrary().GetRootDir() + it->second.m_dae);

		collada::Geometry& refGeo = cs.GetScene().m_geometries.begin()->second;
		{
			data::MemoryFileWriter writer(mf);
			refGeo.Serialize(writer);
		}

		{
			data::MemoryFileReader reader(mf);
			collada::Geometry geo;
			geo.Deserialize(reader);
		}
	}
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}