#include "JobSystemMeta.h"

#include <iostream>

namespace
{
	jobs::JobSystemMeta m_instance;
}

jobs::JobSystemMeta::JobSystemMeta() :
	BaseObjectMeta(nullptr)
{
}

const jobs::JobSystemMeta& jobs::JobSystemMeta::GetInstance()
{
	return m_instance;
}