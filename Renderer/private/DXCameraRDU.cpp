#include "DXCameraRDU.h"

#include "DXCameraRDUMeta.h"

#include "RenderUtils.h"

#include <corecrt_math_defines.h>

namespace 
{
	rendering::DXCamera* m_camera = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_camera)
		{
			m_camera = utils::GetCamera();
		}
	}
}

rendering::DXCameraRDU::DXCameraRDU() :
	RenderDataUpdater(DXCameraRDUMeta::GetInstance())
{
	CacheObjects();
}

rendering::DXCameraRDU::~DXCameraRDU()
{
}

void rendering::DXCameraRDU::Update(jobs::Job* done)
{
	m_camera->UpdateCamBuffer();
	utils::RunSync(done);
}
