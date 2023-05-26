#include "DXShadowMapUpdater.h"

#include "DXShadowMapUpdaterMeta.h"

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
