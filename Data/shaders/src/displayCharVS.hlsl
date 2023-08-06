#include "common_buffers_lib.hlsl"

struct PSInput
{
    float4 position         : SV_POSITION;
    float2 uv               : UV;
    float4 color            : COLOR;
};

struct DisplayCharVSInput
{
    float3 position             : POSITION;
    float2 uv                   : UV;

    float4 instance_position    : INSTANCE_POSITION;
    float4 uv_pos               : UV_POS;
    float4 color                : COLOR;
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

    float2 uvMin = vertexInput.uv_pos.xy;
    float2 uvMax = vertexInput.uv_pos.zw;

    result.uv.x = (1 - vertexInput.uv.x) * uvMin.x + vertexInput.uv.x * uvMax.x;
    result.uv.y = (1 - vertexInput.uv.y) * uvMin.y + vertexInput.uv.y * uvMax.y;
    result.color = vertexInput.color;
    
    return result;
}
