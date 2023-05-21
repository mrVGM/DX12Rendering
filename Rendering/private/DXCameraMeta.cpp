#include "DXCameraMeta.h"

#include "ICameraMeta.h"

namespace
{
	rendering::DXCameraMeta m_meta;
}

rendering::DXCameraMeta::DXCameraMeta() :
	BaseObjectMeta(&ICameraMeta::GetInstance())
{
}

const rendering::DXCameraMeta& rendering::DXCameraMeta::GetInstance()
{
	return m_meta;
}
