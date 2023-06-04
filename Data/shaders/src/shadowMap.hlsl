cbuffer SMSlot : register(b2)
{
    int m_slot;
}

struct PS_OUTPUT
{
    float4 m_sm: SV_Target0;
    float4 m_sqSM: SV_Target1;
};

PS_OUTPUT PSMain(float4 position : SV_POSITION, float depth : DEPTH) : SV_Target
{
    float dx = ddx(depth);
    float dy = ddy(depth);

    float biasedVar = depth * depth + 0.25 * (dx * dx + dy * dy);

    float4 sm = float4(0, 0, 0, 0);
    float4 smSq = float4(0, 0, 0, 0);

    switch (m_slot)
    {
    case 0:
        sm.x = depth;
        smSq.x = biasedVar;
        break;
    case 1:
        sm.y = depth;
        smSq.y = biasedVar;
        break;
    case 2:
        sm.z = depth;
        smSq.z = biasedVar;
        break;
    case 3:
        sm.w = depth;
        smSq.w = biasedVar;
        break;
    }

    PS_OUTPUT output;
    output.m_sm = sm;
    output.m_sqSM = smSq;

    return output;
}
