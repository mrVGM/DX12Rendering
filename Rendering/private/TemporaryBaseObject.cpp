#include "TemporaryBaseObject.h"

#include "RenderUtils.h"

rendering::TemporaryBaseObject::TemporaryBaseObject()
{
}

void rendering::TemporaryBaseObject::CreateObject(jobs::Job* createObjectJob)
{
	jobs::JobSystem* mainJobSystem = rendering::utils::GetMainJobSystem();
	mainJobSystem->ScheduleJob(createObjectJob);
}

rendering::TemporaryBaseObject::~TemporaryBaseObject()
{
	class DestroyBaseObject : public jobs::Job
	{
	private:
		BaseObject* m_baseObject = nullptr;
	public:
		DestroyBaseObject(BaseObject* baseObject) :
			m_baseObject(baseObject)
		{
		}
		void Do() override
		{
			delete m_baseObject;
		}
	};

	jobs::JobSystem* mainJobSystem = rendering::utils::GetMainJobSystem();
	mainJobSystem->ScheduleJob(new DestroyBaseObject(m_object));
}