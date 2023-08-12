#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include "SceneSettings.h"

#include "DataLib.h"
#include "ColladaScene.h"

#include <iostream>

void scene_converter::Boot()
{
	collada::SceneSettings* settings = new collada::SceneSettings();

	collada::SceneSettings::Settings& sceneSettings = settings->GetSettings();


	for (auto it = sceneSettings.m_scenes.begin(); it != sceneSettings.m_scenes.end(); ++it)
	{
		std::cout << "Converting " << it->second.m_dae << " ..." << std::endl;
		collada::ColladaScene cs;
		cs.Load(data::GetLibrary().GetRootDir() + it->second.m_dae);

		data::MemoryFile mf;
		std::string file = data::GetLibrary().GetRootDir() + it->second.m_binFile;

		data::MemoryFileWriter writer(mf);
		cs.GetScene().Serialize(writer);

		mf.SaveToFile(file);

		std::cout << it->second.m_dae << " converted and saved to " << it->second.m_binFile << "!" << std::endl;
	}
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}