#include "psm_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer PSMSettings : register(b1)
{
    PSMBuffer m_psmBuff;
}

float4 PSMain(float4 position : SV_POSITION, float3 worldPos : WORLD_POSITION) : SV_Target
{
    return float4(1, 1, 1, 1);

    float4 smValue = CalculatePSM(m_camBuff, m_psmBuff, worldPos);
    smValue /= smValue.w;

    float depth = smValue.z;

    return float4(depth, depth, depth, 1);
}
