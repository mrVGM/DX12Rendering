#pragma once

#include "BaseObjectMeta.h"

namespace rendering
{
	class DXCameraRDUMeta : public BaseObjectMeta
	{
	public:
		static const DXCameraRDUMeta& GetInstance();
		DXCameraRDUMeta();
	};
}