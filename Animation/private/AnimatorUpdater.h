#pragma once

#include "AsyncTickUpdater.h"

#include <string>

namespace collada
{
	struct Animation;
}

namespace rendering
{
	class DXMutableBuffer;
}

namespace animation
{
	class Animator;
	class AnimatorUpdater : public rendering::AsyncTickUpdater
	{
	private:
		Animator& m_animator;

		rendering::DXMutableBuffer* m_buffer = nullptr;
		double m_time = 0;
		double m_speed = 1;
		const collada::Animation* m_currentAnimation = nullptr;
		std::string m_animationName;

		void StartAnimation(const collada::Animation* animation);
	public:
		AnimatorUpdater(rendering::DXMutableBuffer* buffer, Animator& animator);
		virtual ~AnimatorUpdater();

		void Update(double dt, jobs::Job* done) override;

		void PlayAnimation(const std::string& animName, double speed);
	};
}
