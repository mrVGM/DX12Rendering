#pragma once

#include "TickUpdater.h"

namespace rendering
{
	class DXShadowMapUpdater : public TickUpdater
	{
	public:
		DXShadowMapUpdater();
		virtual ~DXShadowMapUpdater();

		int GetPriority() override;
		void Update(double dt) override;
	};
}