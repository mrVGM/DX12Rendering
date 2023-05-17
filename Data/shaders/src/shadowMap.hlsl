float4 PSMain(float4 position : SV_POSITION, float depth : DEPTH) : SV_Target
{
    return float4(depth, depth, depth, 1);
}
