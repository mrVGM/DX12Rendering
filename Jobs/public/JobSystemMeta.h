#pragma once

#include "BaseObjectMeta.h"

namespace jobs
{
	class JobSystemMeta : public BaseObjectMeta
	{
	public:
		static const JobSystemMeta& GetInstance();
		JobSystemMeta();
	};
}