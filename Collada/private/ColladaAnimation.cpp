#include "ColladaAnimation.h"

#include "ColladaAnimationMeta.h"
#include "XMLReader.h"

#include "SceneBuilderUtils.h"

#include <set>
#include <queue>

namespace
{
	const xml_reader::Node* FindColladaNode(const std::list<xml_reader::Node*>& nodes)
	{
		using namespace xml_reader;

		for (auto it = nodes.begin(); it != nodes.end(); ++it)
		{
			const Node* cur = *it;
			if (cur->m_tagName == "COLLADA")
			{
				return cur;
			}
		}

		return nullptr;
	}

	const xml_reader::Node* FindSceneNode(const xml_reader::Node* node)
	{
		using namespace xml_reader;
		for (auto it = node->m_children.begin(); it != node->m_children.end(); ++it)
		{
			const Node* cur = *it;
			if (cur->m_tagName == "scene")
			{
				return cur;
			}
		}

		return nullptr;
	}

	const xml_reader::Node* FindVisualScenesLibrary(const xml_reader::Node* node)
	{
		using namespace xml_reader;
		for (auto it = node->m_children.begin(); it != node->m_children.end(); ++it)
		{
			const Node* cur = *it;
			if (cur->m_tagName == "library_visual_scenes")
			{
				return cur;
			}
		}

		return nullptr;
	}

	const xml_reader::Node* FindVisualScene(const xml_reader::Node* scene, const xml_reader::Node* colladaNode)
	{
		using namespace xml_reader;

		const Node* visualSceneInstance = FindChildNode(scene, [](const Node* node) {
			if (node->m_tagName == "instance_visual_scene")
			{
				return true;
			}
			return false;
		});

		std::string url = visualSceneInstance->m_tagProps.find("url")->second;
		url = url.substr(1);

		const Node* visualScenesLibrary = FindVisualScenesLibrary(colladaNode);

		const Node* visualScene = FindChildNode(visualScenesLibrary, [=](const Node* node) {
			if (node->m_tagName != "visual_scene")
			{
				return false;
			}

			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == url)
			{
				return true;
			}

			return false;
		});

		return visualScene;
	}


	void ReadJoints(const std::list<xml_reader::Node*>& nodes, std::list<const xml_reader::Node*>& outJointNodes)
	{
		using namespace xml_reader;

		const Node* colladaNode = FindColladaNode(nodes);
		const Node* sceneNode = FindSceneNode(colladaNode);

		const Node* visualScene = FindVisualScene(sceneNode, colladaNode);

		FindChildNodes(visualScene, [](const Node* node) {
			if (node->m_tagName != "node")
			{
				return false;
			}

			auto it = node->m_tagProps.find("type");
			if (it == node->m_tagProps.end())
			{
				return false;
			}
			if (it->second != "JOINT")
			{
				return false;
			}
			return true;
		}, outJointNodes);
	}
}

collada::ColladaAnimation::ColladaAnimation() :
	BaseObject(collada::ColladaAnimationMeta::GetInstance())
{
	xml_reader::Boot();
}

bool collada::ColladaAnimation::Load(const std::string& filePath)
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

	std::list<const xml_reader::Node*> jointNodes;
	ReadJoints(rootNodes, jointNodes);

	return res;
}

collada::ColladaAnimation::~ColladaAnimation()
{
}
