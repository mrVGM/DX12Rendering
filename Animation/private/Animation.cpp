#include "Animation.h"

#include "AnimRepo.h"

namespace
{
	animation::AnimRepo* m_animRepo = nullptr;
}

void animation::Boot()
{
	if (m_animRepo)
	{
		return;
	}

	m_animRepo = new AnimRepo();
	m_animRepo->LoadAnimations();
}