#include "ColladaSceneMeta.h"

namespace
{
	collada::ColladaSceneMeta m_instance;
}

collada::ColladaSceneMeta::ColladaSceneMeta() :
	BaseObjectMeta(nullptr)
{
}

const collada::ColladaSceneMeta& collada::ColladaSceneMeta::GetInstance()
{
	return m_instance;
}