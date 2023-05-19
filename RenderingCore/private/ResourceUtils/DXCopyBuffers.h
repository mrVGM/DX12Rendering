#pragma once

#include "BaseObject.h"

#include "Job.h"
#include "JobSystem.h"

#include <d3d12.h>
#include <wrl.h>
#include <string>

namespace rendering
{
	class DXBuffer;
	class DXFence;
	class DXCopyCommandQueue;

	class DXCopyBuffers : public BaseObject
	{
		DXCopyCommandQueue* m_copyCommandQueue = nullptr;

		jobs::JobSystem* m_copyJobSytem = nullptr;
		DXFence* m_copyFence = nullptr;
		UINT64 m_copyCounter = 0;

	public:
		DXCopyBuffers();
		virtual ~DXCopyBuffers();

		void Execute(
			DXBuffer& dst,
			const DXBuffer& src,
			jobs::Job* done
		);
	};
}