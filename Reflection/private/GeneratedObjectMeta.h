#pragma once

#include "BaseObjectMeta.h"

namespace reflection
{
	class GeneratedObjectMeta : public BaseObjectMeta
	{
	public:
		GeneratedObjectMeta(const BaseObjectMeta& parent);
	};
}