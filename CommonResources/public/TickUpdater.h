#pragma once

#include "BaseObject.h"
#include "BaseObjectMeta.h"

#include <DirectXMath.h>
#include <list>

namespace rendering
{
	class TickUpdater : public BaseObject
	{
	public:
		TickUpdater(const BaseObjectMeta& meta);
		virtual ~TickUpdater();

		virtual int GetPriority() = 0;
		virtual void Update(double dt) = 0;
	};
}
