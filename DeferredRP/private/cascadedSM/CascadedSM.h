#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <d3d12.h>

#include <list>

namespace rendering
{
	class DXBuffer;
	class DXTexture;
	class DXDescriptorHeap;
	class DXMaterial;

	class CascadedSM : public BaseObject
	{
	public:
		static const UINT m_resolution;
	private:
		DXBuffer* m_smSettingsBuffer = nullptr;
		DXTexture* m_smTex = nullptr;

		std::list<DXMaterial*> m_shadowMapMaterials;

		std::list<DXTexture*> m_depthTextures;
		DXDescriptorHeap* m_dsDescriptorHeap = nullptr;
		DXDescriptorHeap* m_smDescriptorHeap = nullptr;

		void LoadSettingsBuffer(jobs::Job* done);
		void LoadDepthTextures(jobs::Job* done);
		void LoadSMTexture(jobs::Job* done);
		void LoadSMMaterials(jobs::Job* done);

		void CreateDescriptorHeaps();

	public:
		CascadedSM();
		virtual ~CascadedSM();
		
		void LoadResources(jobs::Job* done);

		void UpdateSMSettings();
		DXTexture* GetShadowMap();
		DXDescriptorHeap* GetDSDescriptorHeap();
		DXDescriptorHeap* GetSMDescriptorHeap();
		DXBuffer* GetSettingsBuffer();

		const std::list<DXMaterial*>& GetShadowMapMaterials();
	};
}