#pragma once

#include "TickUpdater.h"

namespace rendering
{
	class DXCameraUpdater : public TickUpdater
	{
	public:
		DXCameraUpdater();
		virtual ~DXCameraUpdater();

		int GetPriority() override;
		void Update(double dt) override;
	};
}
