cbuffer SMSlot : register(b2)
{
    int m_slot;
}

float4 PSMain(float4 position : SV_POSITION, float depth : DEPTH) : SV_Target
{
    float dx = ddx(depth);
    float dy = ddy(depth);

    float biasedVar = depth * depth + 0.25 * (dx * dx + dy * dy);

    return float4(depth, biasedVar, 0, 1);
}
