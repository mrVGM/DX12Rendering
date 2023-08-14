float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    return float4(uv, 0, 0.6);
}
