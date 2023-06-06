#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

Texture2D p_normal       : register(t0);

SamplerState p_sampler  : register(s0);

static const float m_outlineSize = 1;

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float3x3 sobel = float3x3(
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    );

    float3 mainNormal = p_normal.Sample(p_sampler, uv);

    if (dot(mainNormal, mainNormal) == 0)
    {
        return float4(0, 0, 0, 0);
    }

    float2 pixelSize = 1.0 / m_camBuff.m_resolution;
    float horizontal = 0;
    float vertical = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float3 curNormal = p_normal.Sample(p_sampler, uv + m_outlineSize * pixelSize * float2(i, j));

            float curDot = dot(mainNormal, curNormal);

            {
                int row = j + 1;
                int col = i + 1;
                float coef = sobel[row][col];
                horizontal += coef * curDot;
            }

            {
                int row = i + 1;
                int col = j + 1;
                float coef = sobel[row][col];
                horizontal += coef * curDot;
            }
        }
    }

    float d = max(abs(horizontal), abs(vertical));

    d = smoothstep(0.1, 0.15, d);
    
    return float4(0, 0, 0, d);
}
