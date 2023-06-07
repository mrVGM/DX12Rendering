#include "DXCameraRDU.h"

#include "DXCameraRDUMeta.h"

#include "RenderUtils.h"

#include <corecrt_math_defines.h>

namespace 
{
	rendering::DXCamera* m_camera = nullptr;
	rendering::DXMutableBuffer* m_cameraBuffer = nullptr;

	void CacheObjects()
	{
		using namespace rendering;
		if (!m_camera)
		{
			m_camera = utils::GetCamera();
		}

		if (!m_cameraBuffer)
		{
			m_cameraBuffer = utils::GetCameraBuffer();
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
	m_cameraBuffer->Copy(done);
}
