#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

Texture2D p_srcTex : register(t0);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 color = p_srcTex.Sample(p_sampler, uv);

    return float4(color);
}
