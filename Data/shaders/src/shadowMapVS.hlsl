#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

cbuffer SMBuff : register(b0)
{
    SMBuffer m_smBuff;
};

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

    result.position = CalculateShadowMap(m_smBuff, worldPos);

    float depth = result.position.z / result.position.w;
    result.depth = depth;
    
    return result;
}
