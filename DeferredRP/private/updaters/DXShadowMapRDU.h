#pragma once

#include "RenderDataUpdater.h"

namespace rendering
{
	class DXShadowMapRDU : public RenderDataUpdater
	{
	public:
		DXShadowMapRDU();
		virtual ~DXShadowMapRDU();

		void Update(jobs::Job* done) override;
	};
}
