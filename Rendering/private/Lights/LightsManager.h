#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <list>

namespace rendering
{
	class DXBuffer;
	class DXTexture;

	struct Light
	{
		float m_position[3];
		float m_range;
	};

	class LightsManager : public BaseObject
	{
	private:
		std::list<Light> m_lights;
		DXBuffer* m_lightsBuffer = nullptr;
		DXTexture* m_shadowMap = nullptr;

	public:
		LightsManager();
		virtual ~LightsManager();

		void AddLight(const Light& light);
		void LoadLightsBuffer(jobs::Job* done);
		void LoadShadowMap(jobs::Job* done);

		DXBuffer* GetLightsBuffer();
		DXTexture* GetShadowMap();
	};
}