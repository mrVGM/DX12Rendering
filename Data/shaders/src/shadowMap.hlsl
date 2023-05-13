float4 PSMain(float4 position : SV_POSITION) : SV_Target
{
    float depth = position.z / position.w;
    return float4(depth, depth, depth, 1);
}
