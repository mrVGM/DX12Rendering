#include "ColladaAnimationMeta.h"

namespace
{
	collada::ColladaAnimationMeta m_instance;
}

collada::ColladaAnimationMeta::ColladaAnimationMeta() :
	BaseObjectMeta(nullptr)
{
}

const collada::ColladaAnimationMeta& collada::ColladaAnimationMeta::GetInstance()
{
	return m_instance;
}