#include "ColladaAnimation.h"

#include "ColladaAnimationMeta.h"
#include "XMLReader.h"

#include "SceneBuilderUtils.h"

#include "ColladaEntities.h"

#include <set>
#include <queue>
#include <sstream>

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

	void ReadAnimationInput(const xml_reader::Node* source, std::list<float>& outInputList)
	{
		using namespace xml_reader;

		const Node* accessor = FindChildNode(source, [](const Node* node) {
			if (node->m_tagName == "accessor")
			{
				return true;
			}
			return false;
		});
		std::string floatArrayURL = accessor->m_tagProps.find("source")->second;
		floatArrayURL = floatArrayURL.substr(1);

		const Node* floatArray = FindChildNode(source, [=](const Node* node) {
			if (node->m_tagName != "float_array")
			{
				return false;
			}
			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second != floatArrayURL)
			{
				return false;
			}

			return true;
		});

		std::stringstream ss;
		ss << accessor->m_tagProps.find("count")->second;

		int numCount;
		ss >> numCount;

		int index = 0;
		for (auto it = floatArray->m_data.begin(); it != floatArray->m_data.end(); ++it)
		{
			outInputList.push_back(static_cast<float>((*it)->m_symbolData.m_number));
			++index;
			if (index >= numCount)
			{
				break;
			}
		}
	}

	void ReadInterpolationInput(const xml_reader::Node* source, std::list<std::string>& outInterpolationList)
	{
		using namespace xml_reader;

		const Node* accessor = FindChildNode(source, [](const Node* node) {
			if (node->m_tagName == "accessor")
			{
				return true;
			}
			return false;
		});

		std::string nameArrayURL = accessor->m_tagProps.find("source")->second;
		nameArrayURL = nameArrayURL.substr(1);

		const Node* floatArray = FindChildNode(source, [=](const Node* node) {
			if (node->m_tagName != "Name_array")
			{
				return false;
			}
			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second != nameArrayURL)
			{
				return false;
			}

			return true;
		});

		std::stringstream ss;
		ss << accessor->m_tagProps.find("count")->second;

		int numCount;
		ss >> numCount;

		int index = 0;
		for (auto it = floatArray->m_data.begin(); it != floatArray->m_data.end(); ++it)
		{
			outInterpolationList.push_back((*it)->m_symbolData.m_string);
			++index;
			if (index >= numCount)
			{
				break;
			}
		}
	}

	void ReadTransformInput(const xml_reader::Node* source, std::list<collada::Matrix>& outTransformList)
	{
		using namespace xml_reader;

		const Node* accessor = FindChildNode(source, [](const Node* node) {
			if (node->m_tagName == "accessor")
			{
				return true;
			}
			return false;
		});

		std::string floatArrayURL = accessor->m_tagProps.find("source")->second;
		floatArrayURL = floatArrayURL.substr(1);

		const Node* floatArray = FindChildNode(source, [=](const Node* node) {
			if (node->m_tagName != "float_array")
			{
				return false;
			}
			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second != floatArrayURL)
			{
				return false;
			}

			return true;
		});

		std::stringstream ss;
		ss << accessor->m_tagProps.find("count")->second;

		int matCount;
		ss >> matCount;

		ss.clear();
		ss << accessor->m_tagProps.find("stride")->second;

		int stride;
		ss >> stride;

		auto it = floatArray->m_data.begin();
		for (int i = 0; i < matCount; ++i)
		{
			collada::Matrix& mat = outTransformList.emplace_back();
			
			for (int j = 0; j < stride; ++j)
			{
				mat.m_coefs[j] = static_cast<float>((*it++)->m_symbolData.m_number);
			}
		}
	}


	void ReadAnimationChannel(const xml_reader::Node* animationChannel, std::string& boneName, std::list<collada::KeyFrame>& keyFrames)
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


		std::string inputSourceURL = inputInput->m_tagProps.find("source")->second;
		std::string outputSourceURL = inputOutput->m_tagProps.find("source")->second;
		std::string interpolationSourceURL = inputInterpolation->m_tagProps.find("source")->second;

		inputSourceURL = inputSourceURL.substr(1);
		outputSourceURL = outputSourceURL.substr(1);
		interpolationSourceURL = interpolationSourceURL.substr(1);

		const Node* inputSource = FindChildNode(animationChannel, [=](const Node* node) {
			if (node->m_tagName != "source")
			{
				return false;
			}

			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == inputSourceURL)
			{
				return true;
			}

			return false;
		});

		const Node* outputSource = FindChildNode(animationChannel, [=](const Node* node) {
			if (node->m_tagName != "source")
			{
				return false;
			}

			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == outputSourceURL)
			{
				return true;
			}

			return false;
		});

		const Node* interpolationSource = FindChildNode(animationChannel, [=](const Node* node) {
			if (node->m_tagName != "source")
			{
				return false;
			}

			auto it = node->m_tagProps.find("id");
			if (it == node->m_tagProps.end())
			{
				return false;
			}

			if (it->second == interpolationSourceURL)
			{
				return true;
			}

			return false;
		});

		std::list<float> timeArray;
		std::list<std::string> interpolationArray;
		std::list<collada::Matrix> transformArray;

		ReadAnimationInput(inputSource, timeArray);
		ReadInterpolationInput(interpolationSource, interpolationArray);
		ReadTransformInput(outputSource, transformArray);

		{
			auto timeIt = timeArray.begin();
			auto interpolationIt = interpolationArray.begin();
			auto transformIt = transformArray.begin();

			for (int i = 0; i < timeArray.size(); ++i)
			{
				collada::KeyFrame& k = keyFrames.emplace_back();
				k.m_time = *timeIt++;
				k.m_transform = *transformIt++;
				k.m_interpolation = *interpolationIt++;
			}
		}
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

	const xml_reader::Node* colladaNode = FindColladaNode(rootNodes);
	bool invertAxis = collada::ShouldInvertAxis(colladaNode);

	std::list<const xml_reader::Node*> jointNodes;
	ReadJoints(rootNodes, jointNodes);

	for (auto it = jointNodes.begin(); it != jointNodes.end(); ++it)
	{
		const xml_reader::Node* cur = *it;
		m_animation.m_bones.push_back(cur->m_tagProps.find("name")->second);
		m_animation.m_boneParents.push_back(-1);

		int index = 0;
		for (auto parentIt = jointNodes.begin(); parentIt != jointNodes.end(); ++parentIt)
		{
			if (cur->m_parent == *parentIt)
			{
				m_animation.m_boneParents.back() = index;
				break;
			}
		}
	}

	const xml_reader::Node* animation = GetAnimationNode(rootNodes);

	for (auto it = animation->m_children.begin(); it != animation->m_children.end(); ++it)
	{
		std::string bone;
		std::list<KeyFrame> keyFrames;
		ReadAnimationChannel(*it, bone, keyFrames);

		if (invertAxis)
		{
			for (auto it = keyFrames.begin(); it != keyFrames.end(); ++it)
			{
				KeyFrame& cur = *it;
				cur.m_transform = collada::ChangeAxisOfMatrix(cur.m_transform);
			}
		}

		const xml_reader::Node* curJoint = nullptr;
		for (auto jointIt = jointNodes.begin(); jointIt != jointNodes.end(); ++jointIt)
		{
			if ((*jointIt)->m_tagProps.find("id")->second == bone)
			{
				curJoint = *jointIt;
				break;
			}
		}

		const std::string& jointName = curJoint->m_tagProps.find("sid")->second;
		m_animation.m_channels[jointName] = AnimChannel();
		AnimChannel& animChannel = m_animation.m_channels[jointName];

		animChannel.m_boneName = jointName;
		animChannel.m_keyFrames = std::vector<KeyFrame>(keyFrames.size());
		auto kfIt = keyFrames.begin();
		for (int i = 0; i < keyFrames.size(); ++i)
		{
			animChannel.m_keyFrames[i] = *kfIt++;
		}
	}

	return true;
}

collada::ColladaAnimation::~ColladaAnimation()
{
}

collada::Animation& collada::ColladaAnimation::GetAnimation()
{
	return m_animation;
}