#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <list>

namespace rendering
{
	class DXBuffer;

	struct Light
	{
		float m_position[3];
		float m_range;
	};

	class LightsManager : public BaseObject
	{
	private:
		DXBuffer* m_lightsBuffer = nullptr;
		std::list<Light> m_lights;

	public:
		LightsManager();
		virtual ~LightsManager();

		void AddLight(const Light& light);
		void LoadLightsBuffer(jobs::Job* done);
		DXBuffer* GetLightsBuffer();
	};
}