#include "DXShadowMapRDU.h"

#include "DXShadowMapRDUMeta.h"

#include "LightsManager.h"
#include "LightsManagerMeta.h"

#include "BaseObjectContainer.h"

namespace 
{
	rendering::LightsManager* m_lightsManager = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_lightsManager)
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();
			BaseObject* obj = container.GetObjectOfClass(LightsManagerMeta::GetInstance());

			if (!obj)
			{
				throw "Can't find Lights Manager!";
			}

			m_lightsManager = static_cast<LightsManager*>(obj);
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
	m_lightsManager->UpdateShadowMapSettings();
}