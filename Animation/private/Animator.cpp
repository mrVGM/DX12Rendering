#include "Animator.h"

#include "AnimatorMeta.h"
#include "Animation.h"

#include "utils.h"

namespace
{
	void CacheObjects()
	{
	}
}

animation::Animator::Animator(const std::string& objectName) :
	BaseObject(animation::AnimatorMeta::GetInstance()),
	m_objectName(objectName)
{
	CacheObjects();
	animation::Boot();
}

animation::Animator::~Animator()
{
}
