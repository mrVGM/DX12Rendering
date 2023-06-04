#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer SMBuff : register(b1)
{
    SMBuffer m_smBuffer;
};

Texture2D p_position     : register(t0);
Texture2D p_shadowMap    : register(t1);
Texture2D p_shadowMask   : register(t2);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 pixelVal = p_shadowMask.Sample(p_sampler, uv);
    return pixelVal;
    
    float2x2 m = float2x2(0, 2, 3, 1);

    float2 pixelSize = 1.0 / m_camBuff.m_resolution;

    int2 coord = floor(uv / pixelSize);
    coord %= 2;

    pixelVal *= 4;
    pixelVal = round(pixelVal);

    float threshold = m[coord.x][coord.y];
    if (pixelVal.x > threshold)
    {
        return float4(1, 1, 1, 1);
    }
    return float4(0, 0, 0, 1);
}
