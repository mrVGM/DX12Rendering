#pragma once

#include "BaseObject.h"

#include "Job.h"

#include <d3d12.h>
#include <wrl.h>

class BaseObjectMeta;

namespace rendering
{
	class DXBuffer;
	class DXMutableBuffer : public BaseObject
	{
	private:
		bool m_isLoaded = false;
		bool m_isDirty = false;
		DXBuffer* m_buffer = nullptr;
		DXBuffer* m_uploadBuffer = nullptr;

		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

		void CreateCommandList();
	public:
		DXMutableBuffer(const BaseObjectMeta& meta, UINT64 size, UINT64 stride);
		virtual ~DXMutableBuffer();

		void SetDirty();
		bool IsDirty();

		DXBuffer* GetBuffer();
		DXBuffer* GetUploadBuffer();
		void Load(jobs::Job* done);
		void Upload(jobs::Job* done);

		ID3D12CommandList* GetCopyCommandList();
	};
}