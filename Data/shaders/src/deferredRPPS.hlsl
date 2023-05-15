struct Light
{
    float3 m_position;
    float m_range;
};

cbuffer MVCMatrix : register(b0)
{
    float4x4 m_matrix;
    float4 m_camPos;
};

cbuffer LigthsData : register(b1)
{
    int m_numLights;
    float3 m_placeholder;

    Light m_lights[15];
};

cbuffer ShadowMapSettings : register(b2)
{
    float4x4 m_smMatrix;
    float4 m_smPos;
    float m_smFarPlane;
    float m_smNearPlane;
    float m_smFov;
    float m_smAspect;
};

Texture2D p_diffuse      : register(t0);
Texture2D p_specular     : register(t1);
Texture2D p_normal       : register(t2);
Texture2D p_position     : register(t3);
Texture2D p_shadowMap    : register(t4);

SamplerState p_sampler  : register(s0);

struct PS_OUTPUT
{
    float4 m_ambientLit: SV_Target0;
    float4 m_diffuseLit: SV_Target1;
    float4 m_specularLit: SV_Target2;
};

float testForShadow(float3 position)
{
    float4 testedPoint = float4(position, 1);
    testedPoint = mul(m_smMatrix, testedPoint);
    float pointDepth = testedPoint.z / m_smFarPlane;

    testedPoint /= testedPoint.w;
    float2 coord = (testedPoint.xy + 1) / 2;

    if (coord.x < 0 || coord.x > 1 || coord.y < 0 || coord.y > 1)
    {
        return 0;
    }

    coord = float2(coord.x, 1 - coord.y);
    float4 shadowMap = p_shadowMap.Sample(p_sampler, coord);
    return pointDepth - shadowMap.x;
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
        float3 eyeDir = normalize(m_camPos - positionTex.xyz);
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

    for (int i = 0; i < 3; ++i)
    {
        if (i == 0)
        {
            float shadowTest = testForShadow(positionTex.xyz);
            if (shadowTest > 0.01)
            {
                continue;
            }
        }

        Light cur = m_lights[i];
        float3 offset = cur.m_position - positionTex;

        float dist = length(offset);
        float3 dir = normalize(offset);

        if (dist > cur.m_range)
        {
            continue;
        }

        {
            float cosCoef = max(0, dot(dir, normalTex.xyz));
            color += cosCoef * diffuseTex.xyz;
            color = clamp(color, 0, 1);
        }

        {
            float cosCoef = max(0, dot(dir, reflectedEyeDir));
            specularColor += pow(cosCoef, 16) * specularTex;
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
