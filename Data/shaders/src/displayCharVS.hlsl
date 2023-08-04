#include "common_buffers_lib.hlsl"

struct PSInput
{
    float4 position         : SV_POSITION;
    float2 uv               : UV;
};

struct DisplayCharVSInput
{
    float3 position             : POSITION;
    float2 uv                   : UV;

    float4 instance_position    : INSTANCE_POSITION;
    int char_info               : CHAR_INFO;
};

PSInput VSMain(DisplayCharVSInput vertexInput)
{
    PSInput result;

    result.position = float4(vertexInput.position, 1);
    result.uv = vertexInput.uv;

    return result;
}
