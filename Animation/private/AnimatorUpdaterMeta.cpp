#include "AnimatorUpdaterMeta.h"

#include "AsyncTickUpdaterMeta.h"

namespace
{
	animation::AnimatorUpdaterMeta m_meta;
}

animation::AnimatorUpdaterMeta::AnimatorUpdaterMeta() :
	BaseObjectMeta(&rendering::AsyncTickUpdaterMeta::GetInstance())
{
}

const animation::AnimatorUpdaterMeta& animation::AnimatorUpdaterMeta::GetInstance()
{
	return m_meta;
}
