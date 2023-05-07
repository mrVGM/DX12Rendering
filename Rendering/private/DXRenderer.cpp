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
}


rendering::DXRenderer::DXRenderer() :
	BaseObject(DXRendererMeta::GetInstance())
{
	GetRenderFence();
	GetClearRTRP();
}

rendering::DXRenderer::~DXRenderer()
{
}

void rendering::DXRenderer::Render(jobs::Job* done)
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

	utils::RunSync(done);
}

void rendering::DXRenderer::RenderFrame(jobs::Job* done)
{
	struct Context
	{
		DXRenderer* m_renderer = nullptr;
		jobs::Job* m_done;
	};

	class RenderJob : public jobs::Job
	{
	private:
		Context m_ctx;
		
	public:
		RenderJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_renderer->Render(m_ctx.m_done);
		}
	};

	Context ctx {this, done};
	utils::RunAsync(new RenderJob(ctx));
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