#pragma once

#include "AppEntryPoint.h"

namespace rendering
{
	class RendererEntryPoint : public settings::AppEntryPoint
	{
	public:
		RendererEntryPoint();
		virtual ~RendererEntryPoint();

		void Boot() override;
	};
}
