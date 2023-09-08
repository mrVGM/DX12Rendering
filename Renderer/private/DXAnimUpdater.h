#pragma once

#include "TickUpdater.h"

namespace rendering
{
	class DXAnimUpdater : public TickUpdater
	{
	private:
		int m_index = 0;
	public:
		DXAnimUpdater();
		virtual ~DXAnimUpdater();

		int GetPriority() override;
		void Update(double dt) override;
	};
}
