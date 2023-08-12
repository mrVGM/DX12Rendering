#include "SceneConverter.h"

#include "SceneConverterEntryPoint.h"

#include "SceneSettings.h"

#include "DataLib.h"
#include "ColladaScene.h"

#include "XMLWriter.h"

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
			mat.m_tagName = cur.m_name;

			xml_writer::Node& diffuse = mat.m_children.emplace_back();
			diffuse.m_tagName = "diffuse";

			std::stringstream ss;
			ss << cur.m_diffuseColor[0] << ' '
				<< cur.m_diffuseColor[1] << ' '
				<< cur.m_diffuseColor[2] << ' '
				<< cur.m_diffuseColor[3] << ' ';

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


	for (auto it = sceneSettings.m_scenes.begin(); it != sceneSettings.m_scenes.end(); ++it)
	{
		std::cout << "Converting " << it->second.m_dae << " ..." << std::endl;
		collada::ColladaScene cs;
		cs.Load(data::GetLibrary().GetRootDir() + it->second.m_dae);

		data::MemoryFile mf;
		std::string binFilePath = data::GetLibrary().GetRootDir() + it->second.m_binFile;
		std::string materialsFilePath = data::GetLibrary().GetRootDir() + it->second.m_materialsFile;

		data::MemoryFileWriter writer(mf);
		cs.GetScene().Serialize(writer);

		mf.SaveToFile(binFilePath);

		std::string materialsXML = SerializeMaterials(cs.GetScene().m_materials);
		std::ofstream materialsFile(materialsFilePath);
		materialsFile << materialsXML;

		std::cout << it->second.m_dae << " converted and saved to " << it->second.m_binFile << "!" << std::endl;
	}
}

void scene_converter::RegisterLib()
{
	new SceneConverterEntryPoint();
}