#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class CopyJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const CopyJobSystemMeta& GetInstance();
		CopyJobSystemMeta();
	};
}