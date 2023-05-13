float4 PSMain(float4 position : SV_POSITION, float4 worldPosition : WORLD_POSITION) : SV_Target
{
    return float4(worldPosition.w, worldPosition.w, worldPosition.w, 1);
}
