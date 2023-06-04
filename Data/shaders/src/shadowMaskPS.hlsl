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
Texture2D p_shadowSQMap    : register(t2);

SamplerState p_sampler  : register(s0);
SamplerState p_linearSampler  : register(s1);

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

bool hardShadowTest(float2 coords)
{
    float4 positionTex = p_position.Sample(p_sampler, coords);
    int index = GetSMIndex(m_smBuffer, positionTex);
    float2 uv = CalculateShadowMapNormalizedUV(m_smBuffer, index, positionTex);
    float pointDepth = CalculateShadowMapDepth(m_smBuffer, index, positionTex);

    if (uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
    {
        return false;
    }

    float shadowMap = sampleShadowMap(uv, index);
    if (pointDepth > shadowMap)
    {
        return true;
    }

    return false;
}

float2 depthDuv(float2 cameraCoords)
{
    float4 positionTex = p_position.Sample(p_sampler, cameraCoords);
    int index = GetSMIndex(m_smBuffer, positionTex);

    float2 uv = CalculateShadowMapNormalizedUV(m_smBuffer, index, positionTex);
    float depth = CalculateShadowMapDepth(m_smBuffer, index, positionTex);

    float3 uvd = float3(uv, depth);

    float3 uvd_dx = ddx(uvd);
    float3 uvd_dy = ddy(uvd);

    float det = uvd_dx.x * uvd_dy.y - uvd_dy.x * uvd_dx.y;

    float2x2 jinv = float2x2(
         uvd_dy.y, -uvd_dy.x,
        -uvd_dx.y,  uvd_dx.x
    );

    jinv /= det;

    float2 res = mul(jinv, float2(uvd_dx.z, uvd_dy.z));
    return res;
}

float4 pcf(float2 cameraCoords)
{
    float pixelSize = 1.0 / m_smBuffer.m_resolution;

    float4 positionTex = p_position.Sample(p_sampler, cameraCoords);
    int index = GetSMIndex(m_smBuffer, positionTex);
    float refDepth = CalculateShadowMapDepth(m_smBuffer, index, positionTex);
    float2 uv = CalculateShadowMapNormalizedUV(m_smBuffer, index, positionTex);
    float2 depth_duv = depthDuv(cameraCoords);

    float density = 0.0;
    int cnt = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float2 offset = pixelSize * float2(i, j);
            float2 curUV = uv + offset;

            float curSample = sampleShadowMap(curUV, index);
            float adjustedDepth = refDepth + dot(depth_duv, offset);

            density += adjustedDepth > curSample;
            ++cnt;
        }
    }
    density /= cnt;
    float d = 1 - density;

    return float4(d, d, d, 1);
}

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float d = hardShadowTest(uv);
    d = 1 - d;

    return float4(d, d, d, 1);

    float4 test = pcf(uv);
    return test;
    
    return float4(d, d, d, 1);
}
