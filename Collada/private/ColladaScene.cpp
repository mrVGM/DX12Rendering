#include "ColladaScene.h"

#include "ColladaSceneMeta.h"
#include "Collada.h"

#include "XMLReader.h"

#include "SceneBuilderUtils.h"

#include <set>
#include <queue>

namespace
{
	bool loadColladaScene(const std::string& filePath, collada::Scene& scene)
	{

		xml_reader::IXMLReader* reader = xml_reader::GetReader();
		scripting::ISymbol* s = reader->ReadColladaFile(filePath);
		if (!s)
		{
			return false;
		}

		struct ColladaNodesContainer
		{
			std::list<xml_reader::Node*> m_nodes;
			~ColladaNodesContainer()
			{
				for (std::list<xml_reader::Node*>::iterator it = m_nodes.begin();
					it != m_nodes.end(); ++it) {
					delete* it;
				}
				m_nodes.clear();
			}
		};

		ColladaNodesContainer nodesContainer;

		std::list<xml_reader::Node*> rootNodes;
		bool res = reader->ConstructColladaTree(s, rootNodes, nodesContainer.m_nodes);

		if (!res)
		{
			return false;
		}
		AssignParents(rootNodes);

		res = ConvertToScene(rootNodes, scene);

		return res;
	}
}

collada::ColladaScene::ColladaScene() :
	BaseObject(collada::ColladaSceneMeta::GetInstance())
{
	xml_reader::Boot();
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

collada::ColladaNodesContainer::~ColladaNodesContainer()
{
	for (std::list<xml_reader::Node*>::iterator it = m_nodes.begin();
		it != m_nodes.end(); ++it) {
		delete* it;
	}
	m_nodes.clear();
}
