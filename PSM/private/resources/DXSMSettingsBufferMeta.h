#pragma once

#include "BaseObjectMeta.h"

namespace rendering::psm
{
	class DXSMSettingsBufferMeta : public BaseObjectMeta
	{
	public:
		static const DXSMSettingsBufferMeta& GetInstance();
		DXSMSettingsBufferMeta();
	};
}