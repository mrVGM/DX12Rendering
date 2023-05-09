Texture2D p_diffuse     : register(t0);
Texture2D p_normal      : register(t1);
Texture2D p_position    : register(t2);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 tex = p_diffuse.Sample(p_sampler, uv);
    return tex;
}
