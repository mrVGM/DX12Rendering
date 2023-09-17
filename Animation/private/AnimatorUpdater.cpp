#include "AnimatorUpdater.h"

#include "AnimatorUpdaterMeta.h"

#include "ColladaEntities.h"

#include "DXBuffer.h"
#include "DXMutableBuffer.h"

#include "Animator.h"

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

	collada::Matrix SampleAnim(const collada::AnimChannel& channel, double time)
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

		collada::Transform leftTr = channel.m_keyFrames[left].m_transform.ToTransform();
		collada::Transform rightTr = channel.m_keyFrames[right].m_transform.ToTransform();

		float coef = leftOffset / (rightOffset + leftOffset);

		collada::Transform res = collada::Transform::Lerp(leftTr, rightTr, coef);
		return res.ToMatrix();
	}
}

animation::AnimatorUpdater::AnimatorUpdater(rendering::DXMutableBuffer* buffer, animation::Animator& animator) :
	AsyncTickUpdater(AnimatorUpdaterMeta::GetInstance()),
	m_animator(animator),
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

	struct Context
	{
		AnimatorUpdater* m_self = nullptr;
		double m_dt = -1;

		jobs::Job* m_done = nullptr;
	};

	Context ctx{ this, dt, done };

	class PoseUpdate : public jobs::Job
	{
	private:
		Context m_ctx;

	public:
		PoseUpdate(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			AnimatorUpdater* self = m_ctx.m_self;

			self->m_time += self->m_speed * m_ctx.m_dt;
			double lastKF = GetLastKeyFrameTime(*self->m_currentAnimation);

			while (self->m_time < 0)
			{
				self->m_time += lastKF;
			}

			while (self->m_time > lastKF)
			{
				self->m_time -= lastKF;
			}

			DXBuffer* uploadBuff = self->m_buffer->GetUploadBuffer();
			void* data = uploadBuff->Map();
			collada::Matrix* matrixData = reinterpret_cast<collada::Matrix*>(data);

			const collada::Animation& anim = *self->m_currentAnimation;

			const collada::Skeleton* skel = self->m_animator.GetSkeleton();

			for (int i = 0; i < skel->m_joints.size(); ++i)
			{
				auto chanIt = anim.m_channels.find(skel->m_joints[i]);
				const collada::AnimChannel& curChannel = anim.m_channels.find(skel->m_joints[i])->second;

				int boneIndex = -1;
				for (int j = 0; j < anim.m_bones.size(); ++j)
				{
					if (anim.m_bones[j] == skel->m_joints[i])
					{
						boneIndex = j;
						break;
					}
				}

				int curParent = anim.m_boneParents[boneIndex];

				collada::Matrix curTransform = SampleAnim(curChannel, self->m_time);

				while (curParent >= 0)
				{
					const collada::AnimChannel& parentChannel = anim.m_channels.find(anim.m_bones[curParent])->second;

					collada::Matrix parentTransform = SampleAnim(parentChannel, self->m_time);

					curTransform = collada::Matrix::Multiply(curTransform, parentTransform);
					curParent = anim.m_boneParents[curParent];
				}

				*matrixData++ = curTransform;
			}
			uploadBuff->Unmap();

			self->m_buffer->SetDirty();
			rendering::core::utils::RunSync(m_ctx.m_done);
		}
	};

	core::utils::RunAsync(new PoseUpdate(ctx));
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
