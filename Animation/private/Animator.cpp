#include "Animator.h"

#include "AnimatorMeta.h"
#include "Animation.h"

#include "AnimatorUpdater.h"

#include "utils.h"

namespace
{
	rendering::DXScene* m_scene = nullptr;

	void CacheObjects()
	{
		if (!m_scene)
		{
			m_scene = animation::GetScene();
		}
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
