#include "common_buffers_lib.hlsl"

cbuffer SMBuff : register(b0)
{
    SMBuffer m_smBuffer;
};

Texture2D p_srcTex : register(t0);

SamplerState p_sampler  : register(s0);

static const float m_blurScale = 1;

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    //return p_srcTex.Sample(p_sampler, uv);

    float3x3 blurMatrix = float3x3(
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    );
    
    
    float pixelSize = 1.0 / m_smBuffer.m_resolution;
    float offsetSize = m_blurScale * pixelSize;

    float4 color = float4(0, 0, 0, 0);
    int cnt = 0;
    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            float2 curCoords = uv + offsetSize * float2(i, j);
            float4 curSample = p_srcTex.Sample(p_sampler, curCoords);
            color += (blurMatrix[i + 1][j + 1]) * curSample / 16.0;
        }
    }

    return color;
}
