#pragma once

#include "AppEntryPoint.h"

namespace rendering
{
	class RendererEntryPoint : public settings::AppEntryPoint
	{
	public:
		RendererEntryPoint(const BaseObjectMeta& meta);
		virtual ~RendererEntryPoint();

		void Boot() override;
	};
}
