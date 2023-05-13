#include "quat_lib.hlsl"

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

PSInput VSMain(
    float3 position : POSITION,
    float3 normal : NORMAL,
    float2 uv : UV,
    float3 objectPosition : OBJECT_POSITION,
    float4 objectRotation : OBJECT_ROTATION,
    float3 objectScale : OBJECT_SCALE)
{
    PSInput result;
    float3 scaledPos = objectScale * position;
    float3 rotatedPos = rotateVector(scaledPos, objectRotation);
    float3 rotatedNormal = rotateVector(normal, objectRotation);

    float3 pos = objectPosition + rotatedPos;
    result.position = mul(m_matrix, float4(pos, 1));

    float depth = result.position.z / m_farPlane;
    result.world_position = float4(pos, depth);
    result.normal = float4(rotatedNormal, 1);
    result.uv = uv;

    return result;
}
