#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer PSMSettings : register(b1)
{
    PSMBuffer m_psmBuff;
}

struct PSInput
{
    float4 position             : SV_POSITION;
    float3 world_position       : WORLD_POSITION;
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

    result.position = mul(m_camBuff.m_matrix, float4(worldPos, 1));
    result.world_position = worldPos;
    
    return result;
}
