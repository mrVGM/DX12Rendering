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

SamplerState p_sampler  : register(s0);


float sampleShadowMap(float2 uv, int index)
{
    uv = float2(uv.x, 1 - uv.y);
    float4 shadowMap = p_shadowMap.Sample(p_sampler, uv);

    float d = 0;
    switch (index)
    {
    case 0:
        d = shadowMap.x;
        break;
    case 1:
        d = shadowMap.y;
        break;
    case 2:
        d = shadowMap.z;
        break;
    case 3:
        d = shadowMap.w;
        break;
    }

    if (d == 0)
    {
        return 1;
    }
    return d;
}

bool hardShadowTest(float3 position)
{
    int smIndex = GetSMIndex(m_smBuffer, position);

    float pointDepth = CalculateShadowMapDepth(m_smBuffer, smIndex, position);
    float2 coord = CalculateShadowMapNormalizedUV(m_smBuffer, smIndex, position);

    if (coord.x < 0 || coord.x > 1 || coord.y < 0 || coord.y > 1)
    {
        return false;
    }

    float shadowMap = sampleShadowMap(coord, smIndex);
    if (pointDepth > shadowMap)
    {
        return true;
    }

    return false;
}

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 positionTex = p_position.Sample(p_sampler, uv);
    bool test = hardShadowTest(positionTex);

    if (test)
    {
        return float4(0, 0, 0, 1);
    }

    return float4(1, 1, 1, 1);
}
