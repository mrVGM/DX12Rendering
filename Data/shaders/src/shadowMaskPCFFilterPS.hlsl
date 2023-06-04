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

float random(float2 st) {
    float tmp = sin(dot(st.xy,
        float2(12.9898, 78.233))) *
        43758.5453123;

    return tmp - floor(tmp);
}

static const float PI = 3.14159265f;

float4 GaussianBlurShadowMask(float2 coord)
{
    float Pi = 2 * PI; // Pi*2

    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 15.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

    float2 Radius = Size / m_camBuff.m_resolution;

    float4 Color = p_shadowMask.Sample(p_sampler, coord);

    // Blur calculations
    for (float d = 0.0; d < Pi; d += Pi / Directions)
    {
        for (float i = 1.0 / Quality; i <= 1.0; i += 1.0 / Quality)
        {
            Color += p_shadowMask.Sample(p_sampler, coord + float2(cos(d), sin(d)) * Radius * i);
        }
    }

    // Output to screen
    Color /= Quality * Directions - 15.0;
    return Color;
}

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

float pcf(float2 coords, int index, float refDepth)
{
    float pixelSize = 1.0 / m_smBuffer.m_resolution;
    float2 pixelCenter = coords / pixelSize;
    pixelCenter = floor(pixelCenter);
    pixelCenter += 0.5;
    pixelCenter *= pixelSize;

    float density = 0.0;
    for (int i = -2; i <= 2; ++i)
    {
        for (int j = -2; j <= 2; ++j)
        {
            float2 curOffset = float2(i, j);

            float smSample = sampleShadowMap(pixelCenter + pixelSize * curOffset, index);
            density += refDepth > smSample;
        }
    }

    float res = density / 25.0;
    if (res < 0.5)
    {
        return 0.0;
    }

    return (res - 0.5) / 0.5;
}

float bilinearInterpolation(float2 anchor, int index, float2 fractOffset, float refDepth)
{
    float pixelSize = 1.0 / m_smBuffer.m_resolution;

    anchor /= pixelSize;
    float2 tl = anchor;
    float2 tr = anchor + float2(1, 0);
    float2 bl = anchor + float2(0, 1);
    float2 br = anchor + float2(1, 1);

    tl *= pixelSize;
    tr *= pixelSize;
    bl *= pixelSize;
    br *= pixelSize;

    float tld = pcf(tl, index, refDepth);
    float trd = pcf(tr, index, refDepth);
    float bld = pcf(bl, index, refDepth);
    float brd = pcf(br, index, refDepth);

    float top = (1 - fractOffset.x) * tld + fractOffset.x * trd;
    float bottom = (1 - fractOffset.x) * bld + fractOffset.x * brd;

    float res = (1 - fractOffset.y) * top + fractOffset.y * bottom;
    return res;
}

float softShadowTest(float3 position, int index)
{
    float2 coords = CalculateShadowMapNormalizedUV(m_smBuffer, index, position);
    float pointDepth = CalculateShadowMapDepth(m_smBuffer, index, position);

    float pixelSize = 1.0 / m_smBuffer.m_resolution;

    coords /= pixelSize;
    coords += 0.5;

    float2 anchor = floor(coords);
    float2 fractOffset = coords - anchor;

    anchor -= 0.5;
    anchor *= pixelSize;

    float density = 0;
    density += bilinearInterpolation(anchor, index, fractOffset, pointDepth);

    return density;
}


float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 positionTex = p_position.Sample(p_sampler, uv);

    float4 shadowMaskHard = p_shadowMask.Sample(p_sampler, uv);
    float shadowMaskBlurry = GaussianBlurShadowMask(uv);

    return shadowMaskHard;

    float lightIntensity = 1;

    if (shadowMaskHard.x < 1 || shadowMaskBlurry < 1) {
        //if (shadowMaskHard < 1) {
        int index = GetSMIndex(m_smBuffer, positionTex);
        float softShadow = softShadowTest(positionTex, index);
        //bool hardShadow = hardShadowTest(positionTex);
        //lightIntensity = p_shadowMask.Sample(p_sampler, uv);
        lightIntensity = 1 - softShadow;

        /*
        float refDepth = CalculateShadowMapDepth(m_smBuffer, index, positionTex);
        float2 coords = CalculateShadowMapNormalizedUV(m_smBuffer, index, positionTex);
        float pcfTest = pcf(coords, index, refDepth);

        lightIntensity = 1 - pcfTest;
        */
    }

    return float4(lightIntensity, lightIntensity, lightIntensity, 1);
}
