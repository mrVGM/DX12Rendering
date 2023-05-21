#include "ICameraMeta.h"

namespace
{
	rendering::ICameraMeta m_meta;
}

rendering::ICameraMeta::ICameraMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::ICameraMeta& rendering::ICameraMeta::GetInstance()
{
	return m_meta;
}
