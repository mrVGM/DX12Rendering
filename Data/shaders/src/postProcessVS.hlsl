#include "common_buffers_lib.hlsl"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
};

struct DisplayCharVSInput
{
    float3 position : POSITION;
    float2 uv : UV;
};

PSInput VSMain(DisplayCharVSInput vertexInput)
{
    PSInput result;

    float4 outPos = float4(vertexInput.position, 1);
    result.position = outPos;
    result.uv = vertexInput.uv;
    
    return result;
}
