#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCopyFenceMeta : public BaseObjectMeta
	{
	public:
		static const DXCopyFenceMeta& GetInstance();
		DXCopyFenceMeta();
	};
}