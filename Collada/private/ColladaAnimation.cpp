#include "ColladaAnimation.h"

#include "ColladaAnimationMeta.h"
#include "XMLReader.h"

#include "SceneBuilderUtils.h"

#include "ColladaEntities.h"

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

	const xml_reader::Node* GetAnimationNode(const std::list<xml_reader::Node*>& nodes)
	{
		using namespace xml_reader;

		const Node* animationsLibrary = nullptr;
		for (auto it = nodes.begin(); it != nodes.end(); ++it)
		{
			Node* cur = *it;
			animationsLibrary = FindChildNode(cur, [](const Node* node) {
				if (node->m_tagName == "library_animations")
				{
					return true;
				}

				return false;
			});

			if (animationsLibrary)
			{
				break;
			}
		}

		if (!animationsLibrary)
		{
			return nullptr;
		}

		for (auto it = animationsLibrary->m_children.begin(); it != animationsLibrary->m_children.end(); ++it)
		{
			const Node* cur = *it;

			if (cur->m_tagName == "animation")
			{
				return cur;
			}
		}
	}

	struct KeyFrame
	{
		float m_time;
		collada::Matrix m_transform;

	};

	void ReadAnimationChannel(const xml_reader::Node* animationChannel, std::string& boneName, std::list<KeyFrame>& KeyFrame)
	{
		using namespace xml_reader;

		const Node* channel = FindChildNode(animationChannel, [](const Node* node) {
			if (node->m_tagName == "channel")
			{
				return true;
			}

			return false;
		});

		std::string target = channel->m_tagProps.find("target")->second;
		size_t separator = target.find('/');

		boneName = target.substr(0, separator);

		const Node* sampler = FindChildNode(animationChannel, [](const Node* node) {
			if (node->m_tagName == "sampler")
			{
				return true;
			}

			return false;
		});

		const Node* inputInput = FindChildNode(sampler, [](const Node* node) {
			if (node->m_tagName != "input")
			{
				return false;
			}

			auto it = node->m_tagProps.find("semantic");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == "INPUT")
			{
				return true;
			}

			return false;
		});

		const Node* inputOutput = FindChildNode(sampler, [](const Node* node) {
			if (node->m_tagName != "input")
			{
				return false;
			}

			auto it = node->m_tagProps.find("semantic");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == "OUTPUT")
			{
				return true;
			}

			return false;
		});

		const Node* inputInterpolation = FindChildNode(sampler, [](const Node* node) {
			if (node->m_tagName != "input")
			{
				return false;
			}

			auto it = node->m_tagProps.find("semantic");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == "INTERPOLATION")
			{
				return true;
			}

			return false;
		});

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

	const xml_reader::Node* animation = GetAnimationNode(rootNodes);

	for (auto it = animation->m_children.begin(); it != animation->m_children.end(); ++it)
	{
		std::string bone;
		std::list<KeyFrame> keyFrames;
		ReadAnimationChannel(*it, bone, keyFrames);
	}

	return res;
}

collada::ColladaAnimation::~ColladaAnimation()
{
}
