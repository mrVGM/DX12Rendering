#include "CoreUtils.h"

#include "BaseObjectContainer.h"

#include "WindowMeta.h"
#include "DXDeviceMeta.h"

#include "DXCommandQueueMeta.h"
#include "DXCopyCommandQueueMeta.h"

#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"

#include "DXSwapChainMeta.h"

#include "ResourceUtils/DXCopyBuffers.h"
#include "ResourceUtils/DXCopyBuffersMeta.h"

#include "JobSystem.h"

namespace
{
	jobs::JobSystem* m_mainJobSystem = nullptr;
	jobs::JobSystem* m_loadJobSystem = nullptr;
	rendering::DXCopyBuffers* m_copyBuffers = nullptr;


	D3D12_INPUT_ELEMENT_DESC m_3DMaterialInputlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		{ "OBJECT_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
		{ "OBJECT_ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 12, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
		{ "OBJECT_SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 28, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
	};

	D3D12_INPUT_ELEMENT_DESC m_3DSkeletalMeshMaterialInputlayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

		{ "OBJECT_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
		{ "OBJECT_ROTATION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 12, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },
		{ "OBJECT_SCALE", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 28, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA, 0 },

		{ "JOINT_1_INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 2, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINT_2_INDEX", 0, DXGI_FORMAT_R32G32B32A32_SINT, 2, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINT_1_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "JOINT_2_WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
}

namespace rendering::core::utils
{
	Window* GetWindow()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj =  container.GetObjectOfClass(WindowMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Window!";
		}

		Window* wnd = static_cast<Window*>(obj);
		return wnd;
	}

	DXDevice* GetDevice()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj = container.GetObjectOfClass(DXDeviceMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Device!";
		}

		DXDevice* device = static_cast<DXDevice*>(obj);
		return device;
	}

	DXCommandQueue* GetCommandQueue()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj = container.GetObjectOfClass(DXCommandQueueMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Command Queue!";
		}

		DXCommandQueue* commandQueue = static_cast<DXCommandQueue*>(obj);
		return commandQueue;
	}

	DXCopyCommandQueue* GetCopyCommandQueue()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj = container.GetObjectOfClass(DXCopyCommandQueueMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Copy Command Queue!";
		}

		DXCopyCommandQueue* commandQueue = static_cast<DXCopyCommandQueue*>(obj);
		return commandQueue;
	}

	DXSwapChain* GetSwapChain()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		BaseObject* obj = container.GetObjectOfClass(DXSwapChainMeta::GetInstance());
		if (!obj)
		{
			throw "Can't find Swap Chain!";
		}

		DXSwapChain* swapChain = static_cast<DXSwapChain*>(obj);
		return swapChain;
	}


	void CacheJobSystems()
	{
		BaseObjectContainer& container = BaseObjectContainer::GetInstance();

		{
			BaseObject* obj = container.GetObjectOfClass(MainJobSystemMeta::GetInstance());
			if (!obj)
			{
				throw "Can't find Main Job System!";
			}

			m_mainJobSystem = static_cast<jobs::JobSystem*>(obj);
		}

		{
			BaseObject* obj = container.GetObjectOfClass(LoadJobSystemMeta::GetInstance());
			if (!obj)
			{
				throw "Can't find Load Job System!";
			}

			m_loadJobSystem = static_cast<jobs::JobSystem*>(obj);
		}

		{
			BaseObject* obj = container.GetObjectOfClass(DXCopyBuffersMeta::GetInstance());
			if (!obj)
			{
				throw "Can't find Copy Buffers!";
			}

			m_copyBuffers = static_cast<DXCopyBuffers*>(obj);
		}
	}


	void RunSync(jobs::Job* job)
	{
		m_mainJobSystem->ScheduleJob(job);
	}
	void RunAsync(jobs::Job* job)
	{
		m_loadJobSystem->ScheduleJob(job);
	}
	void DisposeBaseObject(BaseObject& baseObject)
	{
		class Dispose : public jobs::Job
		{
		private:
			BaseObject& m_object;
		public:
			Dispose(BaseObject& object) :
				m_object(object)
			{
			}

			void Do() override
			{
				delete& m_object;
			}
		};

		utils::RunSync(new Dispose(baseObject));
	}

	void RunCopyLists(ID3D12CommandList* const* lists, UINT64 numLists, jobs::Job* done)
	{
		m_copyBuffers->Execute(lists, numLists, done);
	}


	void Get3DMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements)
	{
		layout = m_3DMaterialInputlayout;
		numElements = _countof(m_3DMaterialInputlayout);
	}

	void Get3DSkeletalMeshMaterialInputLayout(const D3D12_INPUT_ELEMENT_DESC*& layout, unsigned int& numElements)
	{
		layout = m_3DSkeletalMeshMaterialInputlayout;
		numElements = _countof(m_3DSkeletalMeshMaterialInputlayout);
	}
}