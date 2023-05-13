Texture2D p_shadowMap   : register(t0);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 shadowMap = p_shadowMap.Sample(p_sampler, uv);

    float4 res = shadowMap;
    res = clamp(res, 0, 1);
    return res;
}
