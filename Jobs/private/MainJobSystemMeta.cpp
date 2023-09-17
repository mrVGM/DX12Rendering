#include "MainJobSystemMeta.h"

#include "JobSystemMeta.h"

namespace
{
	jobs::MainJobSystemMeta m_meta;
}

jobs::MainJobSystemMeta::MainJobSystemMeta() :
	BaseObjectMeta(&jobs::JobSystemMeta::GetInstance())
{
}

const jobs::MainJobSystemMeta& jobs::MainJobSystemMeta::GetInstance()
{
	return m_meta;
}
