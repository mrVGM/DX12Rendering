#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include "SceneSettings.h"

#include "DataLib.h"
#include "ColladaScene.h"

#include <iostream>

void scene_converter::Boot()
{
	std::cout << "Scene Converter!" << std::endl;

	collada::SceneSettings* settings = new collada::SceneSettings();

	collada::SceneSettings::Settings& sceneSettings = settings->GetSettings();

	data::MemoryFile mf;
	std::string file = data::GetLibrary().GetRootDir() + "asd.bin";

#if true
	mf.RestoreFromFile(file);

	data::MemoryFileReader reader(mf);
	collada::Scene restored;
	restored.Deserialize(reader);
	bool t = true;
#else

	for (auto it = sceneSettings.m_scenes.begin(); it != sceneSettings.m_scenes.end(); ++it)
	{
		collada::ColladaScene cs;
		cs.Load(data::GetLibrary().GetRootDir() + it->second.m_dae);

		data::MemoryFileWriter writer(mf);
		cs.GetScene().Serialize(writer);

		mf.SaveToFile(file);
	}
#endif
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}