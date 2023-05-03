#include "DXBuffer.h"

#include "DXBufferMeta.h"
#include "RenderUtils.h"

#include "DXHeap.h"

#include "ResourceUtils/DXCopyBuffers.h"

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

void rendering::DXBuffer::CopyData(void* data, int dataSize)
{
	CD3DX12_RANGE readRange(0, 0);

	void* dst = nullptr;

	HRESULT hRes = m_buffer->Map(0, &readRange, &dst);
	if (FAILED(hRes))
	{
		throw "Can't map Vertex Buffer!";
	}

	memcpy(dst, data, dataSize);
	m_buffer->Unmap(0, nullptr);
}


void rendering::DXBuffer::Place(DXHeap* heap, UINT64 heapOffset)
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
		throw "Can't place buffer in the heap!";
	}

	m_heap = heap;
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


void rendering::DXBuffer::CopyBuffer(
	rendering::DXBuffer& destination,
	D3D12_RESOURCE_STATES myState,
	D3D12_RESOURCE_STATES destinationState,
	jobs::Job* done) const
{
	DXCopyBuffers* copyBuffers = utils::GetCopyBuffers();

	copyBuffers->Execute(destination, destinationState, *this, myState, done);
}