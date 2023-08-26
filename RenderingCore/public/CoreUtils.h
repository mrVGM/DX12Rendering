#pragma once

#include "Window.h"
#include "DXDevice.h"
#include "DXCommandQueue.h"
#include "DXCopyCommandQueue.h"
#include "DXSwapChain.h"

#include "Job.h"

#include <d3d12.h>

namespace rendering::core::utils
{
	Window* GetWindow();
	DXDevice* GetDevice();
	DXCommandQueue* GetCommandQueue();
	DXCopyCommandQueue* GetCopyCommandQueue();
	DXSwapChain* GetSwapChain();

	void CacheJobSystems();
	void RunSync(jobs::Job* job);
	void RunAsync(jobs::Job* job);
	void DisposeBaseObject(BaseObject& baseObject);

	void RunCopyLists(ID3D12CommandList* const* lists, UINT64 numLists, jobs::Job* done);

	void Get3DMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements);
	void Get3DSkeletalMeshMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements);
}