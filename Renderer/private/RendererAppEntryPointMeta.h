#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class RendererAppEntryPointMeta : public BaseObjectMeta
	{
	public:
		static const RendererAppEntryPointMeta& GetInstance();
		RendererAppEntryPointMeta();
	};
}