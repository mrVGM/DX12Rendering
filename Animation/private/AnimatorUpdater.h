#pragma once

#include "AsyncTickUpdater.h"

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
	class AnimatorUpdater : public rendering::AsyncTickUpdater
	{
	private:
		rendering::DXMutableBuffer* m_buffer = nullptr;
		int m_frame = 0;
		const collada::Animation* m_currentAnimation = nullptr;

	public:
		AnimatorUpdater(rendering::DXMutableBuffer* buffer);
		virtual ~AnimatorUpdater();

		void Update(double dt, jobs::Job* done) override;

		void StartAnimation(const collada::Animation* animation);
	};
}
