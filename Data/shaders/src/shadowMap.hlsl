cbuffer SMSlot : register(b2)
{
    int m_slot;
}

float4 PSMain(float4 position : SV_POSITION, float depth : DEPTH) : SV_Target
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
    return res;
}
