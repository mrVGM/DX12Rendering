Texture2D p_ambient     : register(t0);
Texture2D p_diffuse     : register(t1);
Texture2D p_specular    : register(t2);

SamplerState p_sampler  : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float4 ambientTex = p_ambient.Sample(p_sampler, uv);
    float4 diffuseTex = p_diffuse.Sample(p_sampler, uv);

    return ambientTex + diffuseTex;
}
