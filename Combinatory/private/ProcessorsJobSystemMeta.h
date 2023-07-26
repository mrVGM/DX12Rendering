#pragma once

#include "BaseObjectMeta.h"

namespace combinatory
{
	class ProcessorsJobSystemMeta : public BaseObjectMeta
	{
	public:
		static const ProcessorsJobSystemMeta& GetInstance();
		ProcessorsJobSystemMeta();
	};
}