#pragma once

#include "BaseObject.h"

#include "Job.h"

namespace rendering
{
	class DXTexture;
}

namespace rendering::shadow_mapping
{

	class ShadowMap : public BaseObject
	{
	public:
		ShadowMap(const BaseObjectMeta& meta);
		virtual ~ShadowMap();
		
		virtual void LoadResources(jobs::Job* done) = 0;
		virtual void RenderShadowMask() = 0;

		virtual DXTexture* GetShadowMask() = 0;
	};
}