#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXSMSettingsBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXSMSettingsBufferMeta& GetInstance();
		DXSMSettingsBufferMeta();
	};
}