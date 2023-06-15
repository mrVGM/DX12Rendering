#pragma once

#include "AppEntryPoint.h"

namespace scene_converter
{
	class SceneConverterEntryPoint : public settings::AppEntryPoint
	{
	public:
		SceneConverterEntryPoint();
		virtual ~SceneConverterEntryPoint();

		void Boot() override;
	};
}
