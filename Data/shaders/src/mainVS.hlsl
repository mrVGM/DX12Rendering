#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

struct PSInput
{
    float4 position         : SV_POSITION;
    float4 world_position   : WORLD_POSITION;
    float4 normal           : NORMAL;
    float2 uv               : UV;
};

PSInput VSMain(VertexInput3D vertexInput)
{
    PSInput result;

    float3 worldPos;
    float3 worldNormal;
    GetWorldPositonAndNormal(
        vertexInput.position,
        vertexInput.normal,
        vertexInput.objectPosition,
        vertexInput.objectRotation,
        vertexInput.objectScale,

        worldPos,
        worldNormal);

    result.position = mul(m_camBuff.m_matrix, float4(worldPos, 1));

    result.world_position = float4(worldPos, 1);
    result.normal = float4(worldNormal, 1);
    result.uv = vertexInput.uv;

    return result;
}
