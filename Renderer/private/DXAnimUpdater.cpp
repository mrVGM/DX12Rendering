#include "DXAnimUpdater.h"

#include "DXAnimUpdaterMeta.h"

#include "RenderUtils.h"

namespace 
{
	rendering::DXScene* m_scene = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_scene)
		{
			m_scene = utils::GetScene();
		}
	}
}

rendering::DXAnimUpdater::DXAnimUpdater() :
	TickUpdater(DXAnimUpdaterMeta::GetInstance())
{
	CacheObjects();
}

rendering::DXAnimUpdater::~DXAnimUpdater()
{
}

int rendering::DXAnimUpdater::GetPriority()
{
	return 0;
}

void rendering::DXAnimUpdater::Update(double dt)
{
	if (m_scene->m_sceneResources.empty())
	{
		return;
	}
	int index = m_index++;
	m_index %= 250;

	DXScene::SceneResources& sceneRes = m_scene->m_sceneResources.front();
	DXScene::ObjectResources& objRes = sceneRes.m_objectResources.begin()->second;

	DXMutableBuffer* poseBuffer = objRes.m_skeletonPoseBuffer;

	DXBuffer* uploadBuff = poseBuffer->GetUploadBuffer();
	void* data = uploadBuff->Map();
	collada::Matrix* matrixData = reinterpret_cast<collada::Matrix*>(data);

	collada::Animation& anim = m_scene->m_animation;

	if (m_index == anim.m_channels.begin()->second.m_keyFrames.size())
	{
		m_index = 0;
	}

	for (int i = 0; i < anim.m_bones.size(); ++i)
	{
		collada::AnimChannel& curChannel = anim.m_channels[anim.m_bones[i]];
		int curParent = anim.m_boneParents[i];

		collada::Matrix curTransform = curChannel.m_keyFrames[index].m_transform;

		while (curParent >= 0)
		{
			collada::AnimChannel& parentChannel = anim.m_channels[anim.m_bones[curParent]];
			const collada::Matrix& parentTransform = parentChannel.m_keyFrames[index].m_transform;

			curTransform = collada::Matrix::Multiply(curTransform, parentTransform);
			curParent = anim.m_boneParents[curParent];
		}

		*matrixData++ = curTransform;
	}
	uploadBuff->Unmap();

	poseBuffer->SetDirty();
}