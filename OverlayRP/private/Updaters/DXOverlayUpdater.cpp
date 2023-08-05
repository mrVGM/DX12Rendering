#include "DXOverlayUpdater.h"

#include "DXOverlayUpdaterMeta.h"


#include "utils.h"

namespace
{
	rendering::overlay::DXOverlayRP* m_overlayRP = nullptr;
	rendering::DXMutableBuffer* m_quadInstanceBuffer = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_overlayRP)
		{
			m_overlayRP = overlay::GetOverlayRP();
		}

		if (!m_quadInstanceBuffer)
		{
			m_quadInstanceBuffer = overlay::GetQuadInstanceBuffer();
		}
	}
}

rendering::overlay::DXOverlayUpdater::DXOverlayUpdater() :
	TickUpdater(DXOverlayUpdaterMeta::GetInstance())
{
	CacheObjects();
}

rendering::overlay::DXOverlayUpdater::~DXOverlayUpdater()
{
}

int rendering::overlay::DXOverlayUpdater::GetPriority()
{
	return 0;
}

void rendering::overlay::DXOverlayUpdater::Update(double dt)
{
	if (m_init)
	{
		return;
	}

	m_init = true;
}
