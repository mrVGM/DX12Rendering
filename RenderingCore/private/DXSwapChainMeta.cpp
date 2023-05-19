#include "DXSwapChainMeta.h"

namespace
{
	rendering::DXSwapChainMeta m_meta;
}

rendering::DXSwapChainMeta::DXSwapChainMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::DXSwapChainMeta& rendering::DXSwapChainMeta::GetInstance()
{
	return m_meta;
}
