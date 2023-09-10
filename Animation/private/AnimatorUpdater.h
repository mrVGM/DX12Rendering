#pragma once

#include "AsyncTickUpdater.h"

namespace animation
{
	class AnimatorUpdater : public rendering::AsyncTickUpdater
	{
	public:
		AnimatorUpdater(const BaseObjectMeta& meta);
		virtual ~AnimatorUpdater();

		void Update(double dt, jobs::Job* done) override;
	};
}
