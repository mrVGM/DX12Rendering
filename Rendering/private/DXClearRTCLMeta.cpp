#include "DXClearRTCLMeta.h"

namespace
{
	rendering::DXClearRTCLMeta m_meta;
}

rendering::DXClearRTCLMeta::DXClearRTCLMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXClearRTCLMeta& rendering::DXClearRTCLMeta::GetInstance()
{
	return m_meta;
}
