#pragma once

#include "BaseObject.h"

#include "Job.h"

namespace rendering
{
	class TemporaryBaseObject
	{
	public:
		BaseObject* m_object = nullptr;
		void CreateObject(jobs::Job* createObjectJob);

		TemporaryBaseObject();
		virtual ~TemporaryBaseObject();
	};
}