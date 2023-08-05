#include "FontDescriptorHeapMeta.h"

#include "DXDescriptorHeapMeta.h"

namespace
{
	rendering::overlay::FontDescriptorHeapMeta m_meta;
}

rendering::overlay::FontDescriptorHeapMeta::FontDescriptorHeapMeta() :
	BaseObjectMeta(&DXDescriptorHeapMeta::GetInstance())
{
}

const rendering::overlay::FontDescriptorHeapMeta& rendering::overlay::FontDescriptorHeapMeta::GetInstance()
{
	return m_meta;
}
