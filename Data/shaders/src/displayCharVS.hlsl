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

    float4 outPos = float4(vertexInput.position, 1);
    
    if (outPos.x < 0)
    {
        outPos.x = vertexInput.instance_position.x;
    }
    else
    {
        outPos.x = vertexInput.instance_position.z;
    }

    if (outPos.y < 0)
    {
        outPos.y = vertexInput.instance_position.y;
    }
    else
    {
        outPos.y = vertexInput.instance_position.w;
    }

    result.position = outPos;
    result.uv = vertexInput.uv;

    return result;
}
