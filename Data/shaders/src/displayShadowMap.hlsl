Texture2D p_shadowMap   : register(t0);

SamplerState p_sampler  : register(s0);

float4 GaussianBlur(float2 coord)
{
    float Pi = 6.28318530718; // Pi*2

    // GAUSSIAN BLUR SETTINGS {{{
    float Directions = 16.0; // BLUR DIRECTIONS (Default 16.0 - More is better but slower)
    float Quality = 3.0; // BLUR QUALITY (Default 4.0 - More is better but slower)
    float Size = 8.0; // BLUR SIZE (Radius)
    // GAUSSIAN BLUR SETTINGS }}}

    float2 Radius = Size / float2(600, 400);

    float4 Color = p_shadowMap.Sample(p_sampler, coord);

    // Blur calculations
    for (float d = 0.0; d < Pi; d += Pi / Directions)
    {
        for (float i = 1.0 / Quality; i <= 1.0; i += 1.0 / Quality)
        {
            Color += p_shadowMap.Sample(p_sampler, coord + float2(cos(d), sin(d)) * Radius * i);
        }
    }

    // Output to screen
    Color /= Quality * Directions - 15.0;
    return Color;
}

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 shadowMap = GaussianBlur(uv);
    return shadowMap;
    
    if (uv.x > 0.4 || uv.y < 0.6)
    {
        return float4(0, 0, 0, 0);
    }

    uv.y -= 0.6;
    uv /= 0.4;


    float3 res = shadowMap.z;
    res = clamp(res, 0, 1);
    return float4(res,1);
}
