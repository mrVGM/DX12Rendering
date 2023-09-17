#include "FrontendManagerMeta.h"

#include "JobSystemMeta.h"

namespace
{
	frontend::FrontendManagerMeta m_meta;
}

frontend::FrontendManagerMeta::FrontendManagerMeta() :
	BaseObjectMeta(nullptr)
{
}

const frontend::FrontendManagerMeta& frontend::FrontendManagerMeta::GetInstance()
{
	return m_meta;
}
