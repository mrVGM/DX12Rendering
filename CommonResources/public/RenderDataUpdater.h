#pragma once

#include "BaseObject.h"
#include "BaseObjectMeta.h"

#include "Job.h"

#include <DirectXMath.h>
#include <list>

namespace rendering
{
	class RenderDataUpdater : public BaseObject
	{
	public:
		RenderDataUpdater(const BaseObjectMeta& meta);
		virtual ~RenderDataUpdater();

		virtual void Update(jobs::Job* done) = 0;
	};
}
