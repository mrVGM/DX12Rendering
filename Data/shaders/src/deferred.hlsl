cbuffer DeferredSettings : register(b1)
{
    float4 m_color;
};

struct PS_OUTPUT
{
    float4 m_diffuse: SV_Target0;
    float4 m_specular: SV_Target1;
    float4 m_normal: SV_Target2;
    float4 m_position: SV_Target3;
};

PS_OUTPUT PSMain(float4 position : SV_POSITION, float4 worldPosition : WORLD_POSITION, float4 normal : NORMAL)
{
    PS_OUTPUT output;
    output.m_diffuse = m_color;
    output.m_specular = float4(1,1,1,1);
    output.m_normal = normal;
    output.m_position = worldPosition;

    return output;
}
