#pragma once

#include "BaseObject.h"
#include "RenderPass.h"

#include "d3dx12.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXPostProcessRP : public RenderPass
	{
		void Create();
		void RenderPP();

		void CreateQuadVertexBuffer(jobs::Job* done);
		void CreateQuadIndexBuffer(jobs::Job* done);

		void CreateMaterials(jobs::Job* done);

		void LoadBuffers(jobs::Job* done);
	public:
		DXPostProcessRP();
		virtual ~DXPostProcessRP();

		void Prepare() override;
		void Execute() override;
		void Load(jobs::Job* done) override;
	};
}