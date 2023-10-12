#pragma once

#include "AppEntryPoint.h"

namespace reflection
{
	class ReflectionEntryPoint : public settings::AppEntryPoint
	{
	public:
		ReflectionEntryPoint();
		virtual ~ReflectionEntryPoint();

		void Boot() override;
		void Shutdown() override;
	};
}
