#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer SMBuff : register(b1)
{
    SMBuffer m_smBuff;
};

cbuffer SMSlot : register(b2)
{
    int m_slot;
}

struct PSInput
{
    float4 position     : SV_POSITION;
    float depth       : DEPTH;
};

PSInput VSMain(
    float3 position : POSITION,
    float3 normal : NORMAL,
    float2 uv : UV,
    float3 objectPosition : OBJECT_POSITION,
    float4 objectRotation : OBJECT_ROTATION,
    float3 objectScale : OBJECT_SCALE)
{
    PSInput result;

    float3 worldPos;
    float3 worldNormal;
    GetWorldPositonAndNormal(
        position,
        normal,
        objectPosition,
        objectRotation,
        objectScale,

        worldPos,
        worldNormal);

    result.position = CalculateShadowMap(m_smBuff, m_slot, worldPos);

    float depth = CalculateShadowMapBiasedDepth(m_smBuff, m_slot, worldPos, worldNormal);
    result.depth = depth;
    
    return result;
}
