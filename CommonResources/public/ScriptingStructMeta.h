#pragma once

#include "BaseObjectMeta.h"

namespace scripting
{
	class ScriptingStructMeta : public BaseObjectMeta
	{
	public:
		static const ScriptingStructMeta& GetInstance();
		ScriptingStructMeta();
	};
}