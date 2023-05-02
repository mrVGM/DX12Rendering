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
		TemporaryBaseObject& m_temporaryBaseObject;
	public:
		DestroyBaseObject(TemporaryBaseObject& tempObject) :
			m_temporaryBaseObject(tempObject)
		{
		}
		void Do() override
		{
			delete m_temporaryBaseObject.m_object;
		}
	};

	jobs::JobSystem* mainJobSystem = rendering::utils::GetMainJobSystem();
	mainJobSystem->ScheduleJob(new DestroyBaseObject(*this));
}