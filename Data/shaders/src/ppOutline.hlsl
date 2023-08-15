struct OutlineSettings
{
    int2 m_texSize;
    float2 m_placeholder;
    
    float4 m_color;
    float m_scale;
    float m_depthThreshold;
    float m_normalThreshold;
};

cbuffer SettingsBuff : register(b0)
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
    
    float halfScaleFloor = floor(m_settingsBuff.m_scale * 0.5);
    float halfScaleCeil = ceil(m_settingsBuff.m_scale * 0.5);

    float2 texelSize = 1.0 / m_settingsBuff.m_texSize;
    
    float2 bottomLeftUV = uv - texelSize * halfScaleFloor;
    float2 topRightUV = uv + texelSize * halfScaleCeil;
    
    float2 bottomRightUV = uv + float2(texelSize.x * halfScaleCeil, - texelSize.y * halfScaleFloor);
    float2 topLeftUV = uv + float2(-texelSize.x * halfScaleFloor, texelSize.y * halfScaleCeil);
    
    
    // Add to the fragment shader, just below float2 topLeftUV.
    float depth0 = p_cameraDepth.Sample(p_sampler, bottomLeftUV).y;
    float depth1 = p_cameraDepth.Sample(p_sampler, topRightUV).y;
    float depth2 = p_cameraDepth.Sample(p_sampler, bottomRightUV).y;
    float depth3 = p_cameraDepth.Sample(p_sampler, topLeftUV).y;
    
    float depthFiniteDifference0 = depth1 - depth0;
    float depthFiniteDifference1 = depth3 - depth2;
    
    float edgeDepth = sqrt(pow(depthFiniteDifference0, 2) + pow(depthFiniteDifference1, 2));
    
    edgeDepth = edgeDepth > m_settingsBuff.m_depthThreshold ? 1 : 0;
    
    float3 normal0 = p_normals.Sample(p_sampler, bottomLeftUV).xyz;
    float3 normal1 = p_normals.Sample(p_sampler, topRightUV).xyz;
    float3 normal2 = p_normals.Sample(p_sampler, bottomRightUV).xyz;
    float3 normal3 = p_normals.Sample(p_sampler, topLeftUV).xyz;
    
    
    float3 normalFiniteDifference0 = normal1 - normal0;
    float3 normalFiniteDifference1 = normal3 - normal2;

    float edgeNormal = sqrt(dot(normalFiniteDifference0, normalFiniteDifference0) + dot(normalFiniteDifference1, normalFiniteDifference1));
    edgeNormal = edgeNormal > m_settingsBuff.m_normalThreshold ? 1 : 0;
    
    float edge = max(edgeDepth, edgeNormal);
    
    float4 res = float4(edge, edge, edge, 1);
    res *= m_settingsBuff.m_color;
    
    return res;
}
