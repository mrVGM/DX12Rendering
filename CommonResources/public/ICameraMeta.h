#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class ICameraMeta : public BaseObjectMeta
	{
	public:
		static const ICameraMeta& GetInstance();
		ICameraMeta();
	};
}