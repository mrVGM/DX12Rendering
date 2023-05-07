#include "Updater.h"

#include "UpdaterMeta.h"

#include "RenderUtils.h"

rendering::Updater::Updater() :
	BaseObject(rendering::UpdaterMeta::GetInstance())
{
}

rendering::Updater::~Updater()
{
}


void rendering::NotifyUpdater::Do()
{
	Updater* updater = utils::GetUpdater();
	--updater->m_updatesToWaitFor;

	updater->TryStartUpdate();
}

void rendering::Updater::TryStartUpdate()
{
	if (m_updatesToWaitFor > 0)
	{
		return;
	}

	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	auto nowNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto lastTickNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_lastTick);
	long long deltaNN = nowNN.time_since_epoch().count() - lastTickNN.time_since_epoch().count();
	double dt = deltaNN / 1000000000.0;
	m_lastTick = now;

	StartUpdate(dt);
}

void rendering::Updater::StartUpdate(double dt)
{
	m_updatesToWaitFor = 1;

	DXRenderer* renderer = utils::GetRenderer();
	renderer->RenderFrame(new NotifyUpdater());
}

void rendering::Updater::Start()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	m_lastTick = now;
	StartUpdate(0);
}