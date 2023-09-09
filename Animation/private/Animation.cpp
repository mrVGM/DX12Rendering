#include "Animation.h"

#include "AnimRepo.h"

void animation::Boot()
{
	AnimRepo* animRepo = new AnimRepo();
	animRepo->LoadAnimations();
}