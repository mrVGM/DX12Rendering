#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class CascadedSMSettingsMeta : public BaseObjectMeta
	{
	public:
		static const CascadedSMSettingsMeta& GetInstance();
		CascadedSMSettingsMeta();
	};

}