#include "AnimatorMeta.h"

namespace
{
	animation::AnimatorMeta m_meta;
}

animation::AnimatorMeta::AnimatorMeta() :
	BaseObjectMeta(nullptr)
{
}

const animation::AnimatorMeta& animation::AnimatorMeta::GetInstance()
{
	return m_meta;
}
