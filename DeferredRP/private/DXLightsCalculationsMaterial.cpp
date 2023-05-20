#include "DXLightsCalculationsMaterial.h"

#include "DXShader.h"
#include "DXDescriptorHeap.h"

#include "DXLightsCalculationsMaterialMeta.h"

#include "BaseObjectContainer.h"

#include "DXBuffer.h"
#include "DXTexture.h"

#include "DXCameraBufferMeta.h"
#include "DXLightsBufferMeta.h"
#include "DXShadowMapMeta.h"

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
    rendering::DXBuffer* m_cameraBuffer = nullptr;
    rendering::DXBuffer* m_lightsBuffer = nullptr;
    rendering::DXTexture* m_shadowMap = nullptr;

    void CacheObjects()
    {
        using namespace rendering;
        BaseObjectContainer& container = BaseObjectContainer::GetInstance();

        if (!m_cameraBuffer)
        {
            BaseObject* obj = container.GetObjectOfClass(DXCameraBufferMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Camera Buffer!";
            }

            m_cameraBuffer = static_cast<DXBuffer*>(obj);
        }

        if (!m_lightsBuffer)
        {
            BaseObject* obj = container.GetObjectOfClass(DXLightsBufferMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Lights Buffer!";
            }

            m_lightsBuffer = static_cast<DXBuffer*>(obj);
        }

        if (!m_shadowMap)
        {
            BaseObject* obj = container.GetObjectOfClass(DXShadowMapMeta::GetInstance());

            if (!obj)
            {
                throw "Can't find Shadow Map!";
            }

            m_shadowMap = static_cast<DXTexture*>(obj);
        }
    }
}


rendering::DXLightsCalculationsMaterial::DXLightsCalculationsMaterial(const rendering::DXShader& vertexShader, const rendering::DXShader& pixelShader) :
    DXMaterial(DXLightsCalculationsMaterialMeta::GetInstance(), vertexShader, pixelShader)
{
}

rendering::DXLightsCalculationsMaterial::~DXLightsCalculationsMaterial()
{
}

ID3D12CommandList* rendering::DXLightsCalculationsMaterial::GenerateCommandList(
    const DXBuffer& vertexBuffer,
    const DXBuffer& indexBuffer,
    const DXBuffer& instanceBuffer,
    UINT startIndex,
    UINT indexCount,
    UINT instanceIndex)
{
    return nullptr;
}

#undef THROW_ERROR