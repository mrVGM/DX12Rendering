#include "DXShadowMapUpdater.h"

#include "DXShadowMapUpdaterMeta.h"

#include "utils.h"

namespace
{
	rendering::psm::PSM* m_psm = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_psm)
		{
			m_psm = psm::GetPSM();
		}
	}
}

rendering::psm::DXShadowMapUpdater::DXShadowMapUpdater() :
	TickUpdater(DXShadowMapUpdaterMeta::GetInstance())
{
	CacheObjects();
}

rendering::psm::DXShadowMapUpdater::~DXShadowMapUpdater()
{
}

int rendering::psm::DXShadowMapUpdater::GetPriority()
{
	return 10;
}

void rendering::psm::DXShadowMapUpdater::Update(double dt)
{
	m_psm->UpdateSMSettings();
	m_psm->UpdateSMSettings1();
}
