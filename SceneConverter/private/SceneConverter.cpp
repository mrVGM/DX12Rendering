#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include "SceneSettings.h"

#include "DataLib.h"
#include "ColladaScene.h"
#include "ColladaAnimation.h"

#include "XMLWriter.h"
#include "XMLReader.h"

#include <sstream>
#include <iostream>
#include <fstream>

namespace
{
	std::string SerializeMaterials(const std::map<std::string, collada::ColladaMaterial>& colladaMaterials)
	{
		xml_writer::Node settings;
		settings.m_tagName = "settings";
		
		for (auto it = colladaMaterials.begin(); it != colladaMaterials.end(); ++it)
		{
			const collada::ColladaMaterial& cur = it->second;

			xml_writer::Node& mat = settings.m_children.emplace_back();
			mat.m_tagName = "material";
			mat.m_tagProps["id"] = cur.m_name;

			xml_writer::Node& diffuse = mat.m_children.emplace_back();
			diffuse.m_tagName = "diffuse";

			std::stringstream ss;
			ss << cur.m_diffuseColor[0] << ' '
				<< cur.m_diffuseColor[1] << ' '
				<< cur.m_diffuseColor[2] << ' '
				<< cur.m_diffuseColor[3];

			diffuse.m_content = ss.str();
		}

		std::string res = settings.ToString();
		return res;
	}
}

void scene_converter::Boot()
{
	collada::SceneSettings* settings = new collada::SceneSettings();

	collada::SceneSettings::Settings& sceneSettings = settings->GetSettings();

	for (auto it = sceneSettings.m_scenesToConvert.begin(); it != sceneSettings.m_scenesToConvert.end(); ++it)
	{
		collada::SceneSettings::SceneInfo& cur = sceneSettings.m_scenes[*it];

		std::cout << "Converting " << cur.m_dae << " ..." << std::endl;
		collada::ColladaScene* cs = new collada::ColladaScene();

		cs->Load(data::GetLibrary().GetRootDir() + cur.m_dae);

		data::MemoryFile mf;
		std::string binFilePath = data::GetLibrary().GetRootDir() + cur.m_binFile;
		std::string materialsFilePath = data::GetLibrary().GetRootDir() + cur.m_materialsFile;

		data::MemoryFileWriter writer(mf);
		cs->GetScene().Serialize(writer);

		mf.SaveToFile(binFilePath);

		std::string materialsXML = SerializeMaterials(cs->GetScene().m_materials);
		std::ofstream materialsFile(materialsFilePath);
		materialsFile << materialsXML;

		std::cout << cur.m_dae << " converted and saved to " << cur.m_binFile << "!" << std::endl;
	}

	for (auto it = sceneSettings.m_animationsToConvert.begin(); it != sceneSettings.m_animationsToConvert.end(); ++it)
	{
		collada::SceneSettings::AnimationInfo& cur = sceneSettings.m_animations[*it];

		std::cout << "Converting " << cur.m_dae << " ..." << std::endl;

		collada::ColladaAnimation* ca = new collada::ColladaAnimation();
		ca->Load(data::GetLibrary().GetRootDir() + cur.m_dae);

		std::string binFilePath = data::GetLibrary().GetRootDir() + cur.m_binFile;

		data::MemoryFile mf;
		{
			data::MemoryFileWriter writer(mf);
			ca->GetAnimation().Serialize(writer);
			mf.SaveToFile(binFilePath);
		}
	}
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}