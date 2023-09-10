#include "Animator.h"

#include "AnimatorMeta.h"
#include "Animation.h"

#include "AnimatorUpdater.h"

#include "utils.h"

namespace
{
	rendering::DXScene* m_scene = nullptr;

	void CacheObjects()
	{
		if (!m_scene)
		{
			m_scene = animation::GetScene();
		}
	}
}

animation::Animator::Animator(const std::string& objectName) :
	BaseObject(animation::AnimatorMeta::GetInstance()),
	m_objectName(objectName)
{
	CacheObjects();
	animation::Boot();

	rendering::DXMutableBuffer* buffer = nullptr;
	for (int i = 0; i < m_scene->m_colladaScenes.size(); ++i)
	{
		collada::ColladaScene* cur = m_scene->m_colladaScenes[i];
		rendering::DXScene::SceneResources& curSceneResources = m_scene->m_sceneResources[i];

		auto objResIt = curSceneResources.m_objectResources.find(m_objectName);
		if (objResIt == curSceneResources.m_objectResources.end())
		{
			continue;
		}

		buffer = objResIt->second.m_skeletonPoseBuffer;
		break;
	}

	m_updater = new animation::AnimatorUpdater(buffer);

	m_updater->PlayAnimation("dance", 0.1);
}

animation::Animator::~Animator()
{
}
