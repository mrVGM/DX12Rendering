struct Light
{
    float3 m_position;
    float m_range;
};

cbuffer LigthsData : register(b0)
{
    int m_numLights;
    float3 m_placeholder;

    Light m_lights[15];
};

Texture2D p_diffuse     : register(t0);
Texture2D p_specular    : register(t1);
Texture2D p_normal      : register(t2);
Texture2D p_position    : register(t3);

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

    return output;

    float4 diffuseTex = p_diffuse.Sample(p_sampler, uv);
    float4 specularTex = p_specular.Sample(p_sampler, uv);
    float4 normalTex = p_normal.Sample(p_sampler, uv);
    float4 positionTex = p_normal.Sample(p_sampler, uv);

    if (dot(normalTex, normalTex) < 1)
    {
        //return float4(0,0,0,0);
    }

    float3 color = 0.2 * diffuseTex;

    for (int i = 0; i < m_numLights; ++i)
    {
        Light cur = m_lights[i];
        float3 offset = cur.m_position - positionTex;

        float dist = length(offset);
        float3 dir = normalize(offset);

        if (dist > cur.m_range)
        {
            continue;
        }

        float cosCoef = dot(dir, normalTex);

        if (cosCoef <= 0)
        {
            continue;
        }

        float coef = 1 - dist / cur.m_range;
        coef *= dot(dir, normalTex);
        coef *= 3;

        color += coef * diffuseTex;
        color = clamp(color, 0, 1);
    }

    //return float4(color, 1);
}
