Texture2D p_fontTex     : register(t0);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 fontTex = p_fontTex.Sample(p_sampler, uv);

    return fontTex;
}
