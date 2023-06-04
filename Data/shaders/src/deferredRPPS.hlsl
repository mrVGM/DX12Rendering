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

    for (int i = 0; i < m_numLights; ++i)
    {
        float lightIntensity = 1;

        if (i == 0)
        {
            lightIntensity = p_shadowMask.Sample(p_sampler, uv);
        }

        Light cur = m_lights[i];
        float3 dir = normalize(cur.m_direction);

        float3 diffuseContribution = float3(0, 0, 0);
        {
            float cosCoef = max(0, dot(-dir, normalTex.xyz));
            diffuseContribution = lightIntensity * cosCoef * diffuseTex.xyz;
            color += diffuseContribution;
            color = clamp(color, 0, 1);
        }

        if (dot(diffuseContribution, diffuseContribution) > 0)
        {
            float3 eyeDir = normalize(m_camBuff.m_position - positionTex.xyz);
            eyeDir = normalize(eyeDir);

            float3 midDir = (- dir + eyeDir) / 2;
            midDir = normalize(midDir);

            float cosCoef = max(0, dot(midDir, normalTex.xyz));
            specularColor += lightIntensity * pow(cosCoef, 128) * specularTex;
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
