#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXClearRTCLMeta : public BaseObjectMeta
	{
	public:
		static const DXClearRTCLMeta& GetInstance();
		DXClearRTCLMeta();
	};
}