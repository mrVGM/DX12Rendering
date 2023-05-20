#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXMaterialRepoMeta : public BaseObjectMeta
	{
	public:
		static const DXMaterialRepoMeta& GetInstance();
		DXMaterialRepoMeta();
	};
}