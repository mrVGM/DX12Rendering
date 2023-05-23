#pragma once

#include "RenderDataUpdater.h"

namespace rendering
{
	class DXCameraRDU : public RenderDataUpdater
	{
	public:
		DXCameraRDU();
		virtual ~DXCameraRDU();

		void Update() override;
	};
}
