#pragma once

#include "BaseObject.h"
#include "Job.h"

#include <d3d12.h>
#include <string>
#include <list>

namespace rendering
{
	class DXRenderer : public BaseObject
	{
	private:
		UINT64 m_counter = 1;
		void Render(jobs::Job* done);
	public:
		DXRenderer();
		virtual ~DXRenderer();

		void RenderFrame(jobs::Job* done);
		void LoadRPs(jobs::Job* done);
	};
}