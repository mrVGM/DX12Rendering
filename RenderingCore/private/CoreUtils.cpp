#include "CoreUtils.h"

#include "BaseObjectContainer.h"

#include "WindowMeta.h"
#include "DXDeviceMeta.h"

#include "DXCommandQueueMeta.h"
#include "DXCopyCommandQueueMeta.h"

#include "MainJobSystemMeta.h"
#include "LoadJobSystemMeta.h"

#include "JobSystem.h"

namespace
{
	jobs::JobSystem* m_mainJobSystem = nullptr;
	jobs::JobSystem* m_loadJobSystem = nullptr;
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
}