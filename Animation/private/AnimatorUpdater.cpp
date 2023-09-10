#include "AnimatorUpdater.h"

#include "AnimatorUpdaterMeta.h"

#include "ColladaEntities.h"

#include "DXBuffer.h"
#include "DXMutableBuffer.h"

#include "utils.h"

#include "CoreUtils.h"

namespace
{
	animation::AnimRepo* m_animRepo = nullptr;

	void CacheObjects()
	{
		if (!m_animRepo)
		{
			m_animRepo = animation::GetAnimRepo();
		}
	}
}

animation::AnimatorUpdater::AnimatorUpdater(rendering::DXMutableBuffer* buffer) :
	AsyncTickUpdater(AnimatorUpdaterMeta::GetInstance()),
	m_buffer(buffer)
{
	CacheObjects();
}

animation::AnimatorUpdater::~AnimatorUpdater()
{
}

void animation::AnimatorUpdater::Update(double dt, jobs::Job* done)
{
	using namespace rendering;

	m_currentAnimation = m_animRepo->GetAnimation(m_animationName);

	if (!m_currentAnimation)
	{
		rendering::core::utils::RunSync(done);
		return;
	}

	int index = m_frame++;
	m_frame %= m_currentAnimation->m_channels.begin()->second.m_keyFrames.size();

	DXBuffer* uploadBuff = m_buffer->GetUploadBuffer();
	void* data = uploadBuff->Map();
	collada::Matrix* matrixData = reinterpret_cast<collada::Matrix*>(data);

	const collada::Animation& anim = *m_currentAnimation;

	for (int i = 0; i < anim.m_bones.size(); ++i)
	{
		const collada::AnimChannel& curChannel = anim.m_channels.find(anim.m_bones[i])->second;
		int curParent = anim.m_boneParents[i];

		collada::Matrix curTransform = curChannel.m_keyFrames[index].m_transform;

		while (curParent >= 0)
		{
			const collada::AnimChannel& parentChannel = anim.m_channels.find(anim.m_bones[curParent])->second;
			const collada::Matrix& parentTransform = parentChannel.m_keyFrames[index].m_transform;

			curTransform = collada::Matrix::Multiply(curTransform, parentTransform);
			curParent = anim.m_boneParents[curParent];
		}

		*matrixData++ = curTransform;
	}
	uploadBuff->Unmap();

	m_buffer->SetDirty();
	rendering::core::utils::RunSync(done);
}

void animation::AnimatorUpdater::PlayAnimation(const std::string& animName)
{
	m_animationName = animName;
}

void animation::AnimatorUpdater::StartAnimation(const collada::Animation* animation)
{
	m_frame = 0;
	m_currentAnimation = animation;
}
