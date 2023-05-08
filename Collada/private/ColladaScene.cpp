#include "ColladaScene.h"

#include "ColladaSceneMeta.h"
#include "Collada.h"
#include "ColladaReader.h"

namespace
{
	bool loadColladaScene(const std::string& filePath, collada::Scene& scene)
	{
		using namespace collada;
		ColladaReader& reader = ColladaReader::GetInstance();

		scripting::ISymbol* s = reader.ReadColladaFile(filePath);
		if (!s)
		{
			return false;
		}

		struct ColladaNodesContainer
		{
			std::list<collada::ColladaNode*> m_nodes;
			~ColladaNodesContainer()
			{
				for (std::list<collada::ColladaNode*>::iterator it = m_nodes.begin();
					it != m_nodes.end(); ++it) {
					delete* it;
				}
				m_nodes.clear();
			}
		};

		ColladaNodesContainer nodesContainer;

		std::list<ColladaNode*> rootNodes;
		bool res = reader.ConstructColladaTree(s, rootNodes, nodesContainer.m_nodes);

		if (!res)
		{
			return false;
		}

		res = ConvertToScene(rootNodes, scene);

		for (auto it = scene.m_objects.begin(); it != scene.m_objects.end(); ++it)
		{
			Object& cur = it->second;
			const Geometry& geo = scene.m_geometries.find(cur.m_geometry)->second;
			while (cur.m_materialOverrides.size() < geo.m_materials.size())
			{
				cur.m_materialOverrides.push_back("error");
			}
		}

		return res;
	}
}

collada::ColladaScene::ColladaScene() :
	BaseObject(collada::ColladaSceneMeta::GetInstance())
{
	ColladaReader::GetInstance();
}

bool collada::ColladaScene::Load(const std::string& filePath)
{
	bool res = loadColladaScene(filePath, m_scene);
	if (res)
	{
		m_state = State::Loaded;
	}
	else
	{
		m_state = State::FailedToLoad;
	}
	return res;
}

collada::ColladaScene::~ColladaScene()
{
}

collada::Scene& collada::ColladaScene::GetScene()
{
	return m_scene;
}

collada::ColladaScene::State collada::ColladaScene::GetState()
{
	return m_state;
}