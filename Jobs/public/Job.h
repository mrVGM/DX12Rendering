#pragma once

#include "BaseObject.h"

namespace jobs
{
	class Job
	{
	public:
		virtual void Do() = 0;
		virtual ~Job();
	};
}