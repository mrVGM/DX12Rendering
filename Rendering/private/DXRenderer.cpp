#include "DXRenderer.h"

#include "DXRendererMeta.h"

#include "BaseObjectContainer.h"

#include "RenderUtils.h"

#include "JobSystem.h"
#include "DXFence.h"
#include "RenderFenceMeta.h"
#include "RenderPass/DXClearRTRP.h"
#include "RenderPass/DXClearRTRPMeta.h"
#include "WaitFence.h"

#include "Job.h"

#include <vector>

#include <iostream>

namespace
{
	rendering::DXFence* m_renderFence = nullptr;
	jobs::JobSystem* m_renderJobSystem = nullptr;
	rendering::DXClearRTRP* m_clearRTRP = nullptr;

	rendering::DXFence* GetRenderFence()
	{
		if (m_renderFence)
		{
			return m_renderFence;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::RenderFenceMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXFence(rendering::RenderFenceMeta::GetInstance());
		}

		m_renderFence = static_cast<rendering::DXFence*>(obj);
		return m_renderFence;
	}

	rendering::DXClearRTRP* GetClearRTRP()
	{
		if (m_clearRTRP)
		{
			return m_clearRTRP;
		}
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();
		BaseObject* obj = container.GetObjectOfClass(rendering::DXClearRTRPMeta::GetInstance());
		if (!obj)
		{
			obj = new rendering::DXClearRTRP();
		}

		m_clearRTRP = static_cast<rendering::DXClearRTRP*>(obj);
		return m_clearRTRP;
	}

	class RenderJob : public jobs::Job
	{
	private:
		rendering::DXRenderer& m_renderer;
	public:
		RenderJob(rendering::DXRenderer& renderer, UINT64 signal) :
			m_renderer(renderer)
		{
		}

		void Do() override
		{	
			m_renderer.Render();

			m_renderer.RenderFrame();
		}

		virtual ~RenderJob()
		{
			bool t = true;
		}
	};
}


rendering::DXRenderer::DXRenderer() :
	BaseObject(DXRendererMeta::GetInstance())
{
}

rendering::DXRenderer::~DXRenderer()
{
}

void rendering::DXRenderer::Render()
{
	DXSwapChain* swapChain = utils::GetSwapChain();
	swapChain->UpdateCurrentFrameIndex();

	DXClearRTRP* clearRT = GetClearRTRP();

	clearRT->Prepare();

	DXFence* fence = GetRenderFence();
	WaitFence waitFence(*fence);

	clearRT->Execute();

	RenderUnlit();

	DXCommandQueue* commandQueue = utils::GetCommandQueue();
	commandQueue->GetCommandQueue()->Signal(fence->GetFence(), m_counter);

	waitFence.Wait(m_counter);

	swapChain->Present();

	++m_counter;
}

void rendering::DXRenderer::RenderFrame()
{
	utils::RunSync(new RenderJob(*this, m_counter));
}

void rendering::DXRenderer::StartRendering()
{
	GetRenderFence();
	GetClearRTRP();
	RenderFrame();
}

void rendering::DXRenderer::RenderUnlit()
{
	DXScene* scene = utils::GetScene();
	DXMaterial* mat = utils::GetUnlitMaterial();

	mat->ResetCommandLists();

	for (int i = 0; i < scene->m_scenesLoaded; ++i)
	{
		const collada::ColladaScene& curColladaScene = *scene->m_colladaScenes[i];
		const DXScene::SceneResources& curSceneResources = scene->m_sceneResources[i];

		for (auto it = curSceneResources.m_vertexBuffers.begin(); it != curSceneResources.m_vertexBuffers.end(); ++it)
		{
			DXBuffer* vertBuf = it->second;
			DXBuffer* indexBuf = curSceneResources.m_indexBuffers.find(it->first)->second;
			DXBuffer* instanceBuf = curSceneResources.m_instanceBuffers.find(it->first)->second;

			mat->GenerateCommandList(*vertBuf, *indexBuf, *instanceBuf);
		}
	}

	const std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> >& unlitLists = mat->GetGeneratedCommandLists();

	DXCommandQueue* commandQueue = utils::GetCommandQueue();
	for (auto it = unlitLists.begin(); it != unlitLists.end(); ++it)
	{
		ID3D12CommandList *const tmp[] = { it->Get() };
		commandQueue->GetCommandQueue()->ExecuteCommandLists(1, tmp);
	}
}