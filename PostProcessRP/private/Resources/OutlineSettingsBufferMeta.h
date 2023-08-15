#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class OutlineSettingsBufferMeta : public BaseObjectMeta
	{
	public:
		static const OutlineSettingsBufferMeta& GetInstance();
		OutlineSettingsBufferMeta();
	};
}