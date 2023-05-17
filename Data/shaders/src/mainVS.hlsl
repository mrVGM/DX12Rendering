#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"

cbuffer MVCMatrix : register(b0)
{
    float4x4 m_matrix;
    float3 m_position;
    float m_placeholder;

    float m_farPlane;
    float m_nearPlane;
    float m_fov;
    float m_aspect;
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

    result.position = mul(m_matrix, float4(worldPos, 1));

    float depth = result.position.z / m_farPlane;
    result.world_position = float4(worldPos, depth);
    result.normal = float4(worldNormal, 1);
    result.uv = vertexInput.uv;

    return result;
}
