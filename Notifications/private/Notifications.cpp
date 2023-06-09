#include "Notifications.h"

#include "BaseObjectContainer.h"
#include "Job.h"

#include "NotificationReceiver.h"
#include "NotificationReceiverMeta.h"

#include "utils.h"

namespace
{
	jobs::JobSystem* m_mainJobSystem = nullptr;
}

void notifications::Boot()
{
	m_mainJobSystem = notifications::GetMainJobSystem();
}

void notifications::Notify(const BaseObjectMeta& meta)
{
	if (!meta.IsChildOf(NotificationReceiverMeta::GetInstance()))
	{
		throw "Bad notification!";
	}

	struct Context
	{
		const BaseObjectMeta* m_meta = nullptr;
	};

	class NotifyJob : public jobs::Job
	{
	private:
		Context m_ctx;
	public:
		NotifyJob(const Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			BaseObjectContainer& container = BaseObjectContainer::GetInstance();

			std::list<BaseObject*> objects;
			container.GetAllObjectsOfClass(*m_ctx.m_meta, objects);

			for (auto it = objects.begin(); it != objects.end(); ++it)
			{
				NotificationReceiver* receiver = static_cast<NotificationReceiver*>(*it);
				receiver->Notify();
			}
		}
	};
}