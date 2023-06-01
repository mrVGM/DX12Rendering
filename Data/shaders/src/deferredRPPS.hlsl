#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

struct Light
{
    float3 m_direction;
    float m_range;
};

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer LigthsData : register(b1)
{
    int m_numLights;
    float3 m_placeholder;

    Light m_lights[15];
};

cbuffer SMBuff : register(b2)
{
    SMBuffer m_smBuffer;
};

Texture2D p_diffuse      : register(t0);
Texture2D p_specular     : register(t1);
Texture2D p_normal       : register(t2);
Texture2D p_position     : register(t3);
Texture2D p_shadowMap    : register(t4);
Texture2D p_shadowMask    : register(t5);

SamplerState p_sampler  : register(s0);

struct PS_OUTPUT
{
    float4 m_ambientLit: SV_Target0;
    float4 m_diffuseLit: SV_Target1;
    float4 m_specularLit: SV_Target2;
};

float4 GaussianBlurShadowMask(float2 coord)
{
    float Pi = 6.28318530718; // Pi*2

    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 15.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

    float2 Radius = Size / float2(600, 400);

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

float bilinearInterpolation(float2 anchor, int index, float2 fractOffset, float refDepth)
{
    float pixelSize = 1.0 / m_smBuffer.m_resolution;

    anchor /= pixelSize;
    float tl = anchor;
    float tr = anchor + float2(1, 0);
    float bl = anchor + float2(0, 1);
    float br = anchor + float2(1, 1);

    tl *= pixelSize;
    tr *= pixelSize;
    bl *= pixelSize;
    br *= pixelSize;

    float tld = refDepth > sampleShadowMap(tl, index);
    float trd = refDepth > sampleShadowMap(tr, index);
    float bld = refDepth > sampleShadowMap(bl, index);
    float brd = refDepth > sampleShadowMap(br, index);

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

    float2 tl = floor(coords);
    float2 fractOffset = coords - tl;

    tl -= 0.5;
    float2 tr = tl + float2(1, 0);
    float2 bl = tl + float2(0, 1);
    float2 br = tl + float2(1, 1);

    tl *= pixelSize;
    tr *= pixelSize;
    bl *= pixelSize;
    br *= pixelSize;

    float tld = pointDepth > sampleShadowMap(tl, index);
    float trd = pointDepth > sampleShadowMap(tr, index);
    float bld = pointDepth > sampleShadowMap(bl, index);
    float brd = pointDepth > sampleShadowMap(br, index);

    float top = (1 - fractOffset.x) * tld + fractOffset.x * trd;
    float bottom = (1 - fractOffset.x) * bld + fractOffset.x * brd;

    float res = (1 - fractOffset.y) * top + fractOffset.y * bottom;
    return res;
}

PS_OUTPUT PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    PS_OUTPUT output;


    float4 diffuseTex = p_diffuse.Sample(p_sampler, uv);
    float4 specularTex = p_specular.Sample(p_sampler, uv);
    float4 normalTex = p_normal.Sample(p_sampler, uv);
    float4 positionTex = p_position.Sample(p_sampler, uv);

    output.m_ambientLit = float4(0,0,0,0);
    output.m_diffuseLit = float4(0,0,0,0);
    output.m_specularLit = float4(0,0,0,0);

    if (dot(normalTex, normalTex) < 1)
    {
        return output;
    }

    output.m_ambientLit = float4(0.3 * diffuseTex.xyz, 1);

    float3 color = float3(0, 0, 0);
    float3 specularColor = float3(0, 0, 0);

    float3 reflectedEyeDir = float3(0, 0, 0);
    {
        float3 eyeDir = normalize(m_camBuff.m_position - positionTex.xyz);
        reflectedEyeDir = eyeDir;

        if (dot(eyeDir, normalTex.xyz) < 1)
        {
            float3 y = normalTex.xyz;
            float3 x = normalize(cross(eyeDir, y));
            float3 z = cross(y, x);

            float4x4 mat = float4x4(float4(x, 0), float4(y, 0), float4(z, 0), float4(0, 0, 0, 1));
            float4x4 inv = transpose(mat);

            float4 tmp = mul(mat, float4(eyeDir, 1));
            tmp.x = -tmp.x;
            tmp.z = -tmp.z;
            tmp = mul(inv, tmp);

            reflectedEyeDir = tmp.xyz;
        }
    }

    for (int i = 0; i < m_numLights; ++i)
    {
        float lightIntensity = 1;

        if (i == 0)
        {
            float shadowMaskHard = p_shadowMask.Sample(p_sampler, uv);
            float shadowMaskBlurry = GaussianBlurShadowMask(uv);

            if (shadowMaskHard < 1 || shadowMaskBlurry < 1) {
                int index = GetSMIndex(m_smBuffer, positionTex);
                float softShadow = softShadowTest(positionTex, index);
                //bool hardShadow = hardShadowTest(positionTex);
                //lightIntensity = p_shadowMask.Sample(p_sampler, uv);
                lightIntensity = 1 - softShadow;
            }
        }

        Light cur = m_lights[i];
        float3 dir = normalize(cur.m_direction);

        {
            float cosCoef = max(0, dot(-dir, normalTex.xyz));
            color += lightIntensity * cosCoef * diffuseTex.xyz;
            color = clamp(color, 0, 1);
        }

        {
            float cosCoef = max(0, dot(-dir, reflectedEyeDir));
            specularColor += lightIntensity * pow(cosCoef, 16) * specularTex;
            specularColor = clamp(specularColor, 0, 1);
        }
    }

    if (dot(color, color) > 0)
    {
        output.m_diffuseLit = float4(color, 1);
    }

    if (dot(specularColor, specularColor) > 0)
    {
        output.m_specularLit = float4(specularColor, 1);
    }

    return output;
}
