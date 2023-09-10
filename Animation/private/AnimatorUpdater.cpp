#include "AnimatorUpdater.h"

#include "CoreUtils.h"

animation::AnimatorUpdater::AnimatorUpdater(const BaseObjectMeta& meta) :
	AsyncTickUpdater(meta)
{
}

animation::AnimatorUpdater::~AnimatorUpdater()
{
}

void animation::AnimatorUpdater::Update(double dt, jobs::Job* done)
{
	rendering::core::utils::RunSync(done);
}
