Texture2D p_fontTex     : register(t0);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV, float4 color : COLOR) : SV_Target
{
    float4 fontTex = p_fontTex.Sample(p_sampler, uv);

    float4 res = float4(1, 1, 1, 1);
    res *= float4(1, 1, 1, fontTex.w);
    res *= color;

    return res;
}
