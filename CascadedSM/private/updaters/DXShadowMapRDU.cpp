#include "DXShadowMapRDU.h"

#include "DXShadowMapRDUMeta.h"

#include "utils.h"

#include "BaseObjectContainer.h"

namespace 
{
	rendering::CascadedSM* m_cascadedSM = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_cascadedSM)
		{
			m_cascadedSM = deferred::GetCascadedSM();
		}
	}
}

rendering::DXShadowMapRDU::DXShadowMapRDU() :
	RenderDataUpdater(DXShadowMapRDUMeta::GetInstance())
{
	CacheObjects();
}

rendering::DXShadowMapRDU::~DXShadowMapRDU()
{
}

void rendering::DXShadowMapRDU::Update()
{
	m_cascadedSM->UpdateSMSettings();
}