#pragma once

#include "BaseObject.h"
#include "BaseObjectMeta.h"

#include <DirectXMath.h>
#include <list>

namespace rendering
{
	class RenderDataUpdater : public BaseObject
	{
	public:
		RenderDataUpdater(const BaseObjectMeta& meta);
		virtual ~RenderDataUpdater();

		virtual void Update() = 0;
	};
}
