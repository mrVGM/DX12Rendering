#include "common_buffers_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer DeferredSettings : register(b1)
{
    float4 m_color;
    float3 m_specularColor;
    float m_specularCoef;
};

struct PS_OUTPUT
{
    float4 m_diffuse: SV_Target0;
    float4 m_specular: SV_Target1;
    float4 m_normal: SV_Target2;
    float4 m_position: SV_Target3;
    float2 m_depth: SV_Target4;
};

PS_OUTPUT PSMain(float4 position : SV_POSITION, float4 worldPosition : WORLD_POSITION, float4 normal : NORMAL)
{
    PS_OUTPUT output;
    output.m_diffuse = m_color;
    output.m_specular = float4(m_specularColor, m_specularCoef);
    output.m_normal = normal;
    output.m_position = worldPosition;
    
    float4 depth = mul(m_camBuff.m_matrix, worldPosition);
    depth /= depth.w;
    output.m_depth = float2(depth.z, depth.z * worldPosition.z / m_camBuff.m_farPlane);
    
    return output;
}
