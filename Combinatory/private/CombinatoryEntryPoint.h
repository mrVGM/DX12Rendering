#pragma once

#include "AppEntryPoint.h"

namespace combinatory
{
	class CombinatoryEntryPoint : public settings::AppEntryPoint
	{
	public:
		CombinatoryEntryPoint();
		virtual ~CombinatoryEntryPoint();

		void Boot() override;
	};
}
