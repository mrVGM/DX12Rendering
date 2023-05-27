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

	class CascadedSM : public BaseObject
	{
	public:
		static const UINT m_resolution;
	private:
		DXBuffer* m_matrixBuffer = nullptr;
		DXTexture* m_smTex = nullptr;

		std::list<DXTexture*> m_depthTextures;
		DXDescriptorHeap* m_dsDescriptorHeap = nullptr;

		void LoadMatrixBuffer(jobs::Job* done);
		void LoadDepthTextures(jobs::Job* done);
		void LoadSMTexture(jobs::Job* done);

		void CreateDescriptorHeaps();
	public:
		CascadedSM();
		virtual ~CascadedSM();
		
		void LoadResources(jobs::Job* done);
	};
}