#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class UpdaterMeta : public BaseObjectMeta
	{
	public:
		static const UpdaterMeta& GetInstance();
		UpdaterMeta();
	};
}