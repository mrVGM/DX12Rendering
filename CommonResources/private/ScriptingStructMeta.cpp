#include "ScriptingStructMeta.h"

namespace
{
	scripting::ScriptingStructMeta m_meta;
}

scripting::ScriptingStructMeta::ScriptingStructMeta() :
	BaseObjectMeta(nullptr)
{
}

const scripting::ScriptingStructMeta& scripting::ScriptingStructMeta::GetInstance()
{
	return m_meta;
}
