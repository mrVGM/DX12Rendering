float4 PSMain(float4 position : SV_POSITION, float4 worldPosition : WORLD_POSITION, float4 normal : NORMAL) : SV_Target
{
    return float4(1, 0, 1, 1);
}
