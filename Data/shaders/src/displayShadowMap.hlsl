Texture2D p_shadowMap   : register(t0);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    if (uv.x > 0.4 || uv.y < 0.6)
    {
        return float4(0, 0, 0, 0);
    }

    uv.y -= 0.6;
    uv /= 0.4;

    float4 shadowMap = p_shadowMap.Sample(p_sampler, uv);

    float3 res = shadowMap.z;
    res = clamp(res, 0, 1);
    return float4(res,1);
}
