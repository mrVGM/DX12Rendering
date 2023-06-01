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

float sampleShadowMap(float2 uv, int index)
{
    float4 shadowMap = p_shadowMap.Sample(p_sampler, uv);
    uv = float2(uv.x, 1 - uv.y);

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

float testForShadow(float3 position)
{
    int smIndex = GetSMIndex(m_smBuffer, position);

    float pointDepth = CalculateShadowMapDepth(m_smBuffer, smIndex, position);
    float2 coord = CalculateShadowMapNormalizedUV(m_smBuffer, smIndex, position);

    coord = float2(coord.x, 1 - coord.y);
    if (coord.x < 0 || coord.x > 1 || coord.y < 0 || coord.y > 1)
    {
        return 0;
    }

    float shadowMap = sampleShadowMap(coord, smIndex);
    if (pointDepth < shadowMap)
    {
        return 0;
    }

    float pixelSize = 1.0 / m_smBuffer.m_resolution;

    float shadowStrength = 0;
    int sampleCount = 0;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            int2 indexCoord = int2(i, j) - int2(1, 1);
            float2 curCoord = coord + pixelSize * indexCoord;

            float smSample = sampleShadowMap(curCoord, smIndex);
            if (pointDepth > smSample)
            {
                shadowStrength += 1;
            }
            ++sampleCount;
        }
    }

    return shadowStrength / sampleCount;
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

    if (false)
    {
        int index = GetSMIndex(m_smBuffer, positionTex);

        float4 color = float4(0, 1, 1, 1);
        switch (index)
        {
        case 0:
            color = float4(1, 0, 0, 1);
            break;
        case 1:
            color = float4(0, 1, 0, 1);
            break;
        case 2:
            color = float4(0, 0, 1, 1);
            break;
        case 3:
            color = float4(1, 1, 0, 1);
            break;
        }

        output.m_ambientLit = color;

        //return output;
    }


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
            lightIntensity = p_shadowMask.Sample(p_sampler, uv);
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
