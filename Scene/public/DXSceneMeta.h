#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXSceneMeta : public BaseObjectMeta
	{
	public:
		static const DXSceneMeta& GetInstance();
		DXSceneMeta();
	};
}