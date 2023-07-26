#pragma once

#include "BaseObjectMeta.h"

namespace combinatory
{
	class ResultJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const ResultJobSystemMeta& GetInstance();
		ResultJobSystemMeta();
	};
}