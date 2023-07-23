#pragma once

#include "BaseObjectMeta.h"

namespace combinatory
{
	class CombinatorySettingsMeta : public BaseObjectMeta
	{
	public:
		static const CombinatorySettingsMeta& GetInstance();
		CombinatorySettingsMeta();
	};
}