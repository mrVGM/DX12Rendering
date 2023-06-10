#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXShader;
	class DXBuffer;
	class DXTexture;
	class DXMaterial;
	class DXDescriptorHeap;

	class DXDeferredRP : public RenderPass
	{
	private:
		bool m_listsDirty = true;
		ID3D12CommandList** m_commandListsCache = nullptr;
		int m_numCommandLists = 0;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_startList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_afterRenderSceneList;

		DXDescriptorHeap* m_rtvHeap = nullptr;

		bool m_startListPrepared = false;
		bool m_afterRenderSceneListPrepared = false;
		
		void CreateRTVHeap();

		void PrepareStartList();
		void PrepareAfterRenderSceneList();

		void LoadLightsBuffer(jobs::Job* done);

		void RenderDeferred();

		void LoadGBuffer(jobs::Job* done);

	public:
		DXDeferredRP();
		virtual ~DXDeferredRP();

		void SetListsDirty();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}