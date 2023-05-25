#ifndef __SHADOW_MAP_LIB_HLSL__
#define __SHADOW_MAP_LIB_HLSL__

#include "common_buffers_lib.hlsl"
#include "objects_lib.hlsl"

float4 CalculateShadowMap(SMBuffer smBuffer, float3 worldPos)
{
    return mul(smBuffer.m_matrix, float4(worldPos, 1));
}

float4 CalculateShadowMapUV(SMBuffer smBuffer, float3 worldPos)
{
    float4 proj = CalculateShadowMap(smBuffer, worldPos);
    return proj / proj.w;
}

float2 CalculateShadowMapNormalizedUV(SMBuffer smBuffer, float3 worldPos)
{
    float2 uv = CalculateShadowMapUV(smBuffer, worldPos);
    uv += 1;
    uv /= 2;
    return uv;
}

float CalculateShadowMapDepth(SMBuffer smBuffer, float3 worldPos)
{
    float4 proj = CalculateShadowMap(smBuffer, worldPos);
    return proj.z / proj.w;
}


#endif // __SHADOW_MAP_LIB_HLSL__