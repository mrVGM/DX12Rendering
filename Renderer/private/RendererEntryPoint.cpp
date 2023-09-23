#include "RendererEntryPoint.h"

#include "RendererAppEntryPointMeta.h"

#include "Jobs.h"
#include "Job.h"
#include "JobSystemMeta.h"
#include "MainJobSystemMeta.h"

#include "Window.h"
#include "WindowMeta.h"
#include "DXDeviceMeta.h"

#include "BaseObjectContainer.h"

#include "CoreUtils.h"

#include "Renderer.h"

rendering::RendererEntryPoint::RendererEntryPoint() :
	settings::AppEntryPoint(RendererAppEntryPointMeta::GetInstance())
{
}

rendering::RendererEntryPoint::~RendererEntryPoint()
{
}

void rendering::RendererEntryPoint::Boot()
{
	jobs::Boot();
	rendering::Boot();
}

void rendering::RendererEntryPoint::Shutdown()
{
	class Filter : public BaseObjectFilter
	{
	public:
		bool Condition(const BaseObjectMeta& meta) const override
		{
			if (!meta.IsChildOf(jobs::JobSystemMeta::GetInstance()))
			{
				return false;
			}

			if (&meta == &jobs::MainJobSystemMeta::GetInstance())
			{
				return false;
			}

			return true;
		}
	};

	class NoDeviceFilterAndMainJobSystem : public BaseObjectFilter
	{
	public:
		bool Condition(const BaseObjectMeta& meta) const override
		{
			if (&meta == &rendering::DXDeviceMeta::GetInstance())
			{
				return false;
			}

			if (&meta == &jobs::MainJobSystemMeta::GetInstance())
			{
				return false;
			}

			return true;
		}
	};

	class ShutdownJob : public jobs::Job
	{
	public:
		void Do() override
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();

			std::list<BaseObject*> jobSystems;
			Filter filter;
			container.GetAllObjectsFiltered(filter, jobSystems);

			for (auto it = jobSystems.begin(); it != jobSystems.end(); ++it)
			{
				delete* it;
			}

			std::list<BaseObject*> noDeviceObjects;
			NoDeviceFilterAndMainJobSystem noDevicefilter;
			container.GetAllObjectsFiltered(noDevicefilter, noDeviceObjects);

			for (auto it = noDeviceObjects.begin(); it != noDeviceObjects.end(); ++it)
			{
				delete *it;
			}

			BaseObject* device = container.GetObjectOfClass(rendering::DXDeviceMeta::GetInstance());
			delete device;
		}
	};

	class WaitJob : public jobs::Job
	{
	public:
		void Do() override
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));
			core::utils::RunSync(new ShutdownJob());
		}
	};

	class CloseWindowJob : public jobs::Job
	{
	public:
		void Do() override
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();

			Window* wnd = static_cast<Window*>(container.GetObjectOfClass(rendering::WindowMeta::GetInstance()));
			PostMessage(wnd->m_hwnd, WM_CLOSE, 0, 0);

			core::utils::RunAsync(new WaitJob());
		}
	};

	core::utils::RunSync(new CloseWindowJob());
}
