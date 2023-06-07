#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <d3dx12.h>

class BaseObjectMeta;

namespace rendering
{
	class DXBuffer;
	class DXMutableBuffer : public BaseObject
	{
		DXBuffer* m_buffer = nullptr;
		DXBuffer* m_uploadBuffer = nullptr;
	public:
		DXMutableBuffer(const BaseObjectMeta& meta, UINT64 size, UINT64 stride, const D3D12_RESOURCE_FLAGS& flags);
		virtual ~DXMutableBuffer();

		DXBuffer* GetBuffer();
		DXBuffer* GetUploadBuffer();

		void Load(jobs::Job* done);
		void Copy(jobs::Job* done);
	};
}