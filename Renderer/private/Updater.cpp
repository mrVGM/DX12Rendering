#include "Updater.h"

#include "UpdaterMeta.h"

#include "RenderUtils.h"

#include "BaseObjectContainer.h"

#include "TickUpdaterMeta.h"
#include "TickUpdater.h"

#include "AsyncTickUpdaterMeta.h"
#include "AsyncTickUpdater.h"

#include "DXMutableBufferMeta.h"

#include "CoreUtils.h"

#include <corecrt_math_defines.h>
#include <vector>
#include <list>

rendering::Updater::NotifyUpdater::NotifyUpdater(rendering::Updater& updater) :
	m_updater(updater)
{
}

void rendering::Updater::NotifyUpdater::Do()
{
	--m_updater.m_updatesToWaitFor;
	m_updater.Proceed();
}


rendering::Updater::Updater() :
	BaseObject(rendering::UpdaterMeta::GetInstance())
{
}

rendering::Updater::~Updater()
{
	if (m_copyLists)
	{
		delete[] m_copyLists;
	}
}

void rendering::Updater::StartUpdate()
{
	double dt = TimeStamp();
	m_state = UpdaterState::Render;
	m_updatesToWaitFor = 3;

	DXRenderer* renderer = utils::GetRenderer();
	renderer->RenderFrame(new NotifyUpdater(*this));

	RunAsyncTickUpdaters(dt);
	RunTickUpdaters(dt);
}

void rendering::Updater::Start()
{
	StartUpdate();
}

void rendering::Updater::Proceed()
{
	if (m_updatesToWaitFor > 0)
	{
		return;
	}

	if (m_state == UpdaterState::NotStarted)
	{
		StartUpdate();
		return;
	}

	if (m_state == UpdaterState::Render)
	{
		m_state = UpdaterState::Sync;
		m_updatesToWaitFor = 1;

		SyncMutableBuffers();
		return;
	}

	if (m_state == UpdaterState::Sync)
	{
		StartUpdate();
		return;
	}
}


void rendering::Updater::RunAsyncTickUpdaters(double dt)
{
	struct Context
	{
		int m_updatersToWaitFor = -1;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_done = new NotifyUpdater(*this);

	BaseObjectContainer& container = BaseObjectContainer::GetInstance();

	std::list<BaseObject*> asyncTickUpdaters;
	container.GetAllObjectsOfClass(AsyncTickUpdaterMeta::GetInstance(), asyncTickUpdaters);

	if (asyncTickUpdaters.empty())
	{
		utils::RunSync(ctx->m_done);
		delete ctx;
		return;
	}

	ctx->m_updatersToWaitFor = asyncTickUpdaters.size();

	class UpdaterDone : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		UpdaterDone(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_updatersToWaitFor;
			if (m_ctx.m_updatersToWaitFor > 0)
			{
				return;
			}

			core::utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	for (auto it = asyncTickUpdaters.begin(); it != asyncTickUpdaters.end(); ++it)
	{
		AsyncTickUpdater* cur = static_cast<AsyncTickUpdater*>(*it);
		cur->Update(dt, new UpdaterDone(*ctx));
	}
}

void rendering::Updater::RunTickUpdaters(double dt)
{
	struct Context
	{
		Updater* m_updater = nullptr;
		double m_dt = -1;
		std::list<BaseObject*> m_tickUpdaters;
		jobs::Job* m_done = nullptr;
	};

	Context* ctx = new Context();
	ctx->m_updater = this;
	ctx->m_dt = dt;
	ctx->m_done = new NotifyUpdater(*this);

	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	container.GetAllObjectsOfClass(TickUpdaterMeta::GetInstance(), ctx->m_tickUpdaters);

	if (ctx->m_tickUpdaters.empty())
	{
		utils::RunSync(ctx->m_done);
		delete ctx;
		return;
	}

	class RunTickUpdaters : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		RunTickUpdaters(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			std::vector<TickUpdater*> tickUpdaters;
			for (auto it = m_ctx.m_tickUpdaters.begin(); it != m_ctx.m_tickUpdaters.end(); ++it)
			{
				tickUpdaters.push_back(static_cast<TickUpdater*>(*it));
			}

			for (int i = 0; i < tickUpdaters.size() - 1; ++i)
			{
				for (int j = i; j < tickUpdaters.size(); ++j)
				{
					if (tickUpdaters[i]->GetPriority() > tickUpdaters[j]->GetPriority())
					{
						TickUpdater* tmp = tickUpdaters[i];
						tickUpdaters[i] = tickUpdaters[j];
						tickUpdaters[j] = tmp;
					}
				}
			}

			for (auto it = tickUpdaters.begin(); it != tickUpdaters.end(); ++it)
			{
				(*it)->Update(m_ctx.m_dt);
			}

			utils::RunSync(m_ctx.m_done);
			delete &m_ctx;
		}
	};

	utils::RunAsync(new RunTickUpdaters(*ctx));
}

void rendering::Updater::SyncMutableBuffers()
{	
	std::list<BaseObject*> mutableBuffers;

	BaseObjectContainer& container = BaseObjectContainer::GetInstance();
	container.GetAllObjectsOfClass(DXMutableBufferMeta::GetInstance(), mutableBuffers);

	if (mutableBuffers.empty())
	{
		utils::RunSync(new NotifyUpdater(*this));
		return;
	}

	if (m_numCopyLists < mutableBuffers.size())
	{
		if (m_copyLists)
		{
			delete m_copyLists;
		}

		m_copyLists = new ID3D12CommandList* [mutableBuffers.size()];
	}

	UINT64 index = 0;
	for (auto it = mutableBuffers.begin(); it != mutableBuffers.end(); ++it)
	{
		DXMutableBuffer* cur = static_cast<DXMutableBuffer*>(*it);
		if (!cur->IsDirty())
		{
			continue;
		}

		m_copyLists[index++] = cur->GetCopyCommandList();
	}

	if (index == 0)
	{
		utils::RunSync(new NotifyUpdater(*this));
		return;
	}

	core::utils::RunCopyLists(m_copyLists, index, new NotifyUpdater(*this));
}

double rendering::Updater::TimeStamp()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::chrono::system_clock::time_point lastTickCache = m_lastTick;
	m_lastTick = now;

	if (m_state == UpdaterState::NotStarted)
	{
		return 0;
	}

	auto nowNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto lastTickNN = std::chrono::time_point_cast<std::chrono::nanoseconds>(lastTickCache);
	long long deltaNN = nowNN.time_since_epoch().count() - lastTickNN.time_since_epoch().count();
	double dt = deltaNN / 1000000000.0;

	return dt;
}