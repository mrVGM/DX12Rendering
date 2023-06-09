#include "DXShadowMapUpdater.h"

#include "DXShadowMapUpdaterMeta.h"

#include "utils.h"

#include "BaseObjectContainer.h"

namespace
{
	rendering::ILightsManager* m_lightsManager = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_lightsManager)
		{
			m_lightsManager = deferred::GetLightsManager();
		}
	}
}

rendering::DXShadowMapUpdater::DXShadowMapUpdater() :
	TickUpdater(DXShadowMapUpdaterMeta::GetInstance())
{
	CacheObjects();
}

rendering::DXShadowMapUpdater::~DXShadowMapUpdater()
{
}

int rendering::DXShadowMapUpdater::GetPriority()
{
	return 10;
}

void rendering::DXShadowMapUpdater::Update(double dt)
{
}
