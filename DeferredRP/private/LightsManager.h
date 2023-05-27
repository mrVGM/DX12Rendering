#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <vector>

namespace rendering
{
	class DXBuffer;
	class DXTexture;
	class DXDescriptorHeap;

	struct Light
	{
		float m_direction[3];
		float m_range;
	};

	class LightsManager : public BaseObject
	{
	public:
		static int m_shadowMapResolution;
	private:

		std::vector<Light> m_lights;
		DXBuffer* m_lightsBuffer = nullptr;
		
	public:
		LightsManager();
		virtual ~LightsManager();

		void AddLight(const Light& light);
		void LoadLightsBuffer(jobs::Job* done);
		DXBuffer* GetLightsBuffer();

		const Light& GetLight(int index) const;
	};
}