#include "DXCameraMeta.h"

namespace
{
	rendering::DXCameraMeta m_meta;
}

rendering::DXCameraMeta::DXCameraMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXCameraMeta& rendering::DXCameraMeta::GetInstance()
{
	return m_meta;
}
