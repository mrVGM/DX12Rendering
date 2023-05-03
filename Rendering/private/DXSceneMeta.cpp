#include "DXSceneMeta.h"

namespace
{
	rendering::DXSceneMeta m_meta;
}

rendering::DXSceneMeta::DXSceneMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXSceneMeta& rendering::DXSceneMeta::GetInstance()
{
	return m_meta;
}
