#include "DXBuffer.h"

#include "DXBufferMeta.h"
#include "RenderUtils.h"

#include "DXHeap.h"

#include <list>


void rendering::DXBuffer::SetBufferSizeAndFlags(UINT64 size, D3D12_RESOURCE_FLAGS flags)
{
	m_size = size;
	m_bufferDescription = CD3DX12_RESOURCE_DESC::Buffer(m_size, flags);
}

void rendering::DXBuffer::SetBufferStride(UINT64 stride)
{
	m_stride = stride;
}

rendering::DXBuffer::DXBuffer(const BaseObjectMeta& meta) :
	BaseObject(meta)
{
}

rendering::DXBuffer::~DXBuffer()
{
}

bool rendering::DXBuffer::CopyData(void* data, int dataSize, std::string& errorMessage)
{
	CD3DX12_RANGE readRange(0, 0);

	void* dst = nullptr;

	HRESULT hRes = m_buffer->Map(0, &readRange, &dst);
	if (FAILED(hRes))
	{
		errorMessage = "Can't map Vertex Buffer!";
		return false;
	}

	memcpy(dst, data, dataSize);
	m_buffer->Unmap(0, nullptr);

	return true;
}


bool rendering::DXBuffer::Place(
	DXHeap* heap,
	UINT64 heapOffset,
	std::string& errorMessage)
{
	DXDevice* device = rendering::utils::GetDevice();
	D3D12_HEAP_TYPE heapType = heap->GetDescription().Properties.Type;

	D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_PRESENT;
	if (heapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD)
	{
		initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	if (heapType == D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_READBACK)
	{
		initialState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COPY_DEST;
	}

	HRESULT hRes = device->GetDevice().CreatePlacedResource(heap->GetHeap(), heapOffset, &m_bufferDescription, initialState, nullptr, IID_PPV_ARGS(&m_buffer));
	if (FAILED(hRes))
	{
		errorMessage = "Can't place buffer in the heap!";
		return false;
	}

	m_heap = heap;

	return true;
}

ID3D12Resource* rendering::DXBuffer::GetBuffer() const
{
	return m_buffer.Get();
}

UINT64 rendering::DXBuffer::GetBufferSize() const
{
	return m_size;
}

UINT64 rendering::DXBuffer::GetStride() const
{
	return m_stride;
}

UINT64 rendering::DXBuffer::GetElementCount() const
{
	return m_size / m_stride;
}
