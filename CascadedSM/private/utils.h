#pragma once

#include "ILightsManager.h"
#include "CascadedSM.h"
#include "ICamera.h"
#include "DXScene.h"
#include "DXCameraBufferMeta.h"
#include "DXTexture.h"
#include "DXMaterialRepo.h"
#include "DXBuffer.h"


namespace rendering::cascaded
{
	ILightsManager* GetLightsManager();
	CascadedSM* GetCascadedSM();
	ICamera* GetCamera();
	DXScene* GetScene();
	DXBuffer* GetCameraBuffer();

	DXTexture* GetGBufferDiffuseTex();
	DXTexture* GetGBufferSpecularTex();
	DXTexture* GetGBufferNormalTex();
	DXTexture* GetGBufferPositionTex();

	DXBuffer* GetRenderTextureBuffer();

	DXMaterialRepo* GetMaterialRepo();
}