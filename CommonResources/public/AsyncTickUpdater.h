#pragma once

#include "BaseObject.h"
#include "BaseObjectMeta.h"

#include "Job.h"

namespace rendering
{
	class AsyncTickUpdater : public BaseObject
	{
	public:
		AsyncTickUpdater(const BaseObjectMeta& meta);
		virtual ~AsyncTickUpdater();

		virtual void Update(double dt, jobs::Job* done) = 0;
	};
}
