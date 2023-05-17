#include "common_buffers_lib.hlsl"

struct PSInput
{
    float4 position         : SV_POSITION;
    float2 uv               : UV;
};

PSInput VSMain(VertexInput2D vertexInput)
{
    PSInput result;

    result.position = float4(vertexInput.position, 0, 1);
    result.uv = vertexInput.uv;

    return result;
}
