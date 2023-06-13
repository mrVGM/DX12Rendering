#include "DXShadowMapUpdater.h"

#include "DXShadowMapUpdaterMeta.h"

#include "utils.h"

namespace
{
	rendering::CascadedSM* m_cascadedSM = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_cascadedSM)
		{
			m_cascadedSM = cascaded::GetCascadedSM();
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
	m_cascadedSM->UpdateSMSettings();
}
