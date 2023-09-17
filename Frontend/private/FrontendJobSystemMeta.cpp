#include "FrontendJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	frontend::FrontendJobSystemMeta m_meta;
}

frontend::FrontendJobSystemMeta::FrontendJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const frontend::FrontendJobSystemMeta& frontend::FrontendJobSystemMeta::GetInstance()
{
	return m_meta;
}
