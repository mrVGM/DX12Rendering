cbuffer UnlitSettings : register(b1)
{
    float4 m_color;
};

float4 PSMain(float4 position : SV_POSITION, float4 worldPosition : WORLD_POSITION, float4 normal : NORMAL) : SV_Target
{
    return m_color;
}
