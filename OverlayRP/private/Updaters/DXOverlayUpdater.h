#pragma once

#include "TickUpdater.h"

namespace rendering::overlay
{
	class DXOverlayUpdater : public TickUpdater
	{
	private:
		bool m_init = false;
	public:
		DXOverlayUpdater();
		virtual ~DXOverlayUpdater();

		int GetPriority() override;
		void Update(double dt) override;
	};
}