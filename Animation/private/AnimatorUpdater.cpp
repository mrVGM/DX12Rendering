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

	double GetLastKeyFrameTime(const collada::Animation& animation)
	{
		double maxTime = -1;
		for (auto it = animation.m_channels.begin(); it != animation.m_channels.end(); ++it)
		{
			const collada::AnimChannel& cur = it->second;
			maxTime = max(cur.m_keyFrames.back().m_time, maxTime);
		}

		return maxTime;
	}

	const collada::KeyFrame& GetKeyFrame(const collada::AnimChannel& channel, double time)
	{
		int left = 0;
		int right = channel.m_keyFrames.size() - 1;

		while (right - left > 1)
		{
			int mid = (left + right) / 2;

			if (time < channel.m_keyFrames[mid].m_time)
			{
				right = mid;
			}
			else 
			{
				left = mid;
			}
		}

		double leftOffset = time - channel.m_keyFrames[left].m_time;
		double rightOffset = channel.m_keyFrames[right].m_time - time;

		if (leftOffset < rightOffset)
		{
			return channel.m_keyFrames[left];
		}

		return channel.m_keyFrames[right];
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

	m_time += m_speed * dt;
	double lastKF = GetLastKeyFrameTime(*m_currentAnimation);

	while (m_time < 0)
	{
		m_time += lastKF;
	}

	while (m_time > lastKF)
	{
		m_time -= lastKF;
	}

	DXBuffer* uploadBuff = m_buffer->GetUploadBuffer();
	void* data = uploadBuff->Map();
	collada::Matrix* matrixData = reinterpret_cast<collada::Matrix*>(data);

	const collada::Animation& anim = *m_currentAnimation;

	for (int i = 0; i < anim.m_bones.size(); ++i)
	{
		const collada::AnimChannel& curChannel = anim.m_channels.find(anim.m_bones[i])->second;
		int curParent = anim.m_boneParents[i];

		const collada::KeyFrame& kf = GetKeyFrame(curChannel, m_time);
		collada::Matrix curTransform = kf.m_transform;

		while (curParent >= 0)
		{
			const collada::AnimChannel& parentChannel = anim.m_channels.find(anim.m_bones[curParent])->second;

			const collada::KeyFrame& parentKF = GetKeyFrame(parentChannel, m_time);
			const collada::Matrix& parentTransform = parentKF.m_transform;

			curTransform = collada::Matrix::Multiply(curTransform, parentTransform);
			curParent = anim.m_boneParents[curParent];
		}

		*matrixData++ = curTransform;
	}
	uploadBuff->Unmap();

	m_buffer->SetDirty();
	rendering::core::utils::RunSync(done);
}

void animation::AnimatorUpdater::PlayAnimation(const std::string& animName, double speed)
{
	m_animationName = animName;
	m_speed = speed;
}

void animation::AnimatorUpdater::StartAnimation(const collada::Animation* animation)
{
	m_time = 0;
	m_currentAnimation = animation;
}
