#include "DXMaterial.h"

#include "DXBuffer.h"
#include "DXShader.h"
#include "BaseObjectMeta.h"

#include "RenderUtils.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

rendering::DXMaterial::DXMaterial(const BaseObjectMeta& meta, const DXShader& vertexShader, const DXShader& pixelShader) :
	BaseObject(meta),
	m_vertexShader(vertexShader),
	m_pixelShader(pixelShader)
{
	DXDevice* device = utils::GetDevice();

	THROW_ERROR(
		device->GetDevice().CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)),
		"Can't create Command Allocator!")
}

rendering::DXMaterial::~DXMaterial()
{
}

void rendering::DXMaterial::ResetCommandLists()
{
	m_commandLists.clear();
	THROW_ERROR(
		m_commandAllocator->Reset(),
		"Can't reset Command Allocator!")
}

const std::list<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> >& rendering::DXMaterial::GetGeneratedCommandLists() const
{
	return m_commandLists;
}

#undef THROW_ERROR