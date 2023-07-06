#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class ShaderRepoSettingsMeta : public BaseObjectMeta
	{
	public:
		static const ShaderRepoSettingsMeta& GetInstance();
		ShaderRepoSettingsMeta();
	};
}