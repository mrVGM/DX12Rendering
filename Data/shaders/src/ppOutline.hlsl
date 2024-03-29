#include "common_buffers_lib.hlsl"

struct OutlineSettings
{
    int2 m_texSize;
    float2 m_placeholder;
    
    float4 m_color;
    
    float m_scale;
    float m_depthThreshold;
    float m_normalThreshold;
    float m_angleFactor;
    
    float2 m_distanceLimits;
};

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer SettingsBuff : register(b1)
{
    OutlineSettings m_settingsBuff;
};

Texture2D p_cameraDepth : register(t0);
Texture2D p_normals : register(t1);
Texture2D p_position : register(t2);

SamplerState p_sampler : register(s0);

float4 PSMain(float4 position : SV_POSITION, float2 uv : UV) : SV_Target
{
    float2 depth = p_cameraDepth.Sample(p_sampler, uv);
    float3 worldPos = p_position.Sample(p_sampler, uv);
    
    float halfScaleFloor = floor(m_settingsBuff.m_scale * 0.5);
    float halfScaleCeil = ceil(m_settingsBuff.m_scale * 0.5);

    float2 texelSize = 1.0 / m_settingsBuff.m_texSize;
    
    float2 bottomLeftUV = uv - texelSize * halfScaleFloor;
    float2 topRightUV = uv + texelSize * halfScaleCeil;
    float2 bottomRightUV = uv + float2(texelSize.x * halfScaleCeil, - texelSize.y * halfScaleFloor);
    float2 topLeftUV = uv + float2(-texelSize.x * halfScaleFloor, texelSize.y * halfScaleCeil);
    
    float3 normal0 = p_normals.Sample(p_sampler, bottomLeftUV).xyz;
    float3 normal1 = p_normals.Sample(p_sampler, topRightUV).xyz;
    float3 normal2 = p_normals.Sample(p_sampler, bottomRightUV).xyz;
    float3 normal3 = p_normals.Sample(p_sampler, topLeftUV).xyz;
    
    float3 normalFiniteDifference0 = normal1 - normal0;
    float3 normalFiniteDifference1 = normal3 - normal2;

    float edgeNormal = sqrt(dot(normalFiniteDifference0, normalFiniteDifference0) + dot(normalFiniteDifference1, normalFiniteDifference1));
    edgeNormal = edgeNormal > m_settingsBuff.m_normalThreshold ? 1 : 0;
    
    float depth0 = p_cameraDepth.Sample(p_sampler, bottomLeftUV).y;
    float depth1 = p_cameraDepth.Sample(p_sampler, topRightUV).y;
    float depth2 = p_cameraDepth.Sample(p_sampler, bottomRightUV).y;
    float depth3 = p_cameraDepth.Sample(p_sampler, topLeftUV).y;
    
    float depthFiniteDifference0 = depth1 - depth0;
    float depthFiniteDifference1 = depth3 - depth2;
    
    float3 eyeDir = -m_camBuff.m_fwd;
    float angleFactor = saturate(dot(normal0, eyeDir));
    angleFactor = saturate(angleFactor);
    angleFactor = 1 - angleFactor;
    
    {
        float lowerBound = m_settingsBuff.m_angleFactor;
        if (angleFactor < lowerBound)
        {
            angleFactor = 0;
        }
        else
        {
            angleFactor = (angleFactor - lowerBound) / (1 - lowerBound);
        }
    }
    
    float depthThreshold = (1 - angleFactor) * m_settingsBuff.m_depthThreshold + angleFactor;
    
    float edgeDepth = sqrt(pow(depthFiniteDifference0, 2) + pow(depthFiniteDifference1, 2));
    edgeDepth = edgeDepth > depthThreshold ? 1 : 0;
    
    float edge = max(edgeDepth, edgeNormal);
    
    float camRange = m_camBuff.m_farPlane - m_camBuff.m_nearPlane;
    
    if (edge > 0)
    {
        float d = depth0;
        if (dot(normal0, normal0) < 0.000001)
        {
            d = 1;
        }
        edge = smoothstep(
            0,
            m_settingsBuff.m_distanceLimits.y / camRange,
            d
        );
        
        edge = 1 - edge;
    }
    
    float4 res = edge * m_settingsBuff.m_color;
    
    return res;
}
