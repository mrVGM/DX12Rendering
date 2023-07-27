#include "ItemManagerMeta.h"

namespace
{
	combinatory::ItemManagerMeta m_meta;
}

combinatory::ItemManagerMeta::ItemManagerMeta() :
	BaseObjectMeta(nullptr)
{
}

const combinatory::ItemManagerMeta& combinatory::ItemManagerMeta::GetInstance()
{
	return m_meta;
}
