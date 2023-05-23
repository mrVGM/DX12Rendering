#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCameraMeta : public BaseObjectMeta
	{
	public:
		static const DXCameraMeta& GetInstance();
		DXCameraMeta();
	};
}