#include "psm_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer PSMSettings : register(b1)
{
    PSMBuffer m_psmBuff;
}

Texture2D p_position      : register(t0);
Texture2D p_shadowMap    : register(t1);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float3 worldPos = p_position.Sample(p_sampler, uv);

    float4 sampleDepth = CalculatePSM(m_psmBuff, worldPos);
    sampleDepth /= sampleDepth.w;

    float4 smSample = p_shadowMap.Sample(p_sampler, sampleDepth);

    bool inShadow = smSample.x > sampleDepth.z;
    return float4(inShadow, inShadow, inShadow, 1);
}
