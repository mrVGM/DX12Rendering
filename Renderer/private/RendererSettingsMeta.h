#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class RendererSettingsMeta : public BaseObjectMeta
	{
	public:
		static const RendererSettingsMeta& GetInstance();
		RendererSettingsMeta();
	};
}