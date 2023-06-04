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
    float4 res = float4(0, 0, 0, 0);
    switch (m_slot)
    {
    case 0:
        res.x = depth;
        break;
    case 1:
        res.y = depth;
        break;
    case 2:
        res.z = depth;
        break;
    case 3:
        res.w = depth;
        break;
    }

    PS_OUTPUT output;
    output.m_sm = res;
    output.m_sqSM = res * res;

    return output;
}
