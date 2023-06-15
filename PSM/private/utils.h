#pragma once

#include "ILightsManager.h"
#include "PSM.h"
#include "ICamera.h"
#include "DXScene.h"
#include "DXTexture.h"
#include "DXMaterialRepo.h"
#include "DXBuffer.h"
#include "DXMutableBuffer.h"


namespace rendering::psm
{
	ILightsManager* GetLightsManager();
	PSM* GetPSM();
	ICamera* GetCamera();
	DXScene* GetScene();
	DXMutableBuffer* GetCameraBuffer();

	DXTexture* GetGBufferDiffuseTex();
	DXTexture* GetGBufferSpecularTex();
	DXTexture* GetGBufferNormalTex();
	DXTexture* GetGBufferPositionTex();

	DXBuffer* GetRenderTextureBuffer();

	DXMaterialRepo* GetMaterialRepo();
}