struct OutlineSettings
{
    float4 m_color;
};

cbuffer SettingsBuff : register(b0)
{
    OutlineSettings m_settingsBuff;
};


Texture2D p_cameraDepth : register(t0);

SamplerState p_sampler : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float2 depth = p_cameraDepth.Sample(p_sampler, uv);
    
    float d = depth.x;
    float4 res = float4(d, d, d, 1);
    res *= m_settingsBuff.m_color;
    
    return res;
}
