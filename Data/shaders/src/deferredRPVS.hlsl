#include "quat_lib.hlsl"

cbuffer MVCMatrix : register(b0)
{
    float4x4 m_matrix;
};

struct PSInput
{
    float4 position         : SV_POSITION;
    float2 uv               : UV;
};

PSInput VSMain(
    float2 position : POSITION,
    float2 uv : UV)
{
    PSInput result;

    result.position = float4(position, 0, 1);
    result.uv = uv;

    return result;
}
