#include "AnimRepoMeta.h"

namespace
{
	animation::AnimRepoMeta m_meta;
}

animation::AnimRepoMeta::AnimRepoMeta() :
	BaseObjectMeta(nullptr)
{
}

const animation::AnimRepoMeta& animation::AnimRepoMeta::GetInstance()
{
	return m_meta;
}
