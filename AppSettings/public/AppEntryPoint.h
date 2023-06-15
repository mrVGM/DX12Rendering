#pragma once

#include "BaseObject.h"

#include <string>

namespace settings
{
	class AppEntryPoint : public BaseObject
	{
	public:
		AppEntryPoint(const BaseObjectMeta& meta);
		virtual ~AppEntryPoint();

		virtual void Boot() = 0;
	};
}
