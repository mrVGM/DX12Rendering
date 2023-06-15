#include "PSMMeta.h"

namespace
{
	rendering::psm::PSMMeta m_meta;
}

rendering::psm::PSMMeta::PSMMeta() :
	BaseObjectMeta(nullptr)
{
}

const rendering::psm::PSMMeta & rendering::psm::PSMMeta::GetInstance()
{
	return m_meta;
}
