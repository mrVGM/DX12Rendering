#ifndef __SHADOW_MAP_LIB_HLSL__
#define __SHADOW_MAP_LIB_HLSL__

#include "common_buffers_lib.hlsl"
#include "objects_lib.hlsl"

float4 CalculateShadowMap(SMBuffer smBuffer, float3 worldPos)
{
    return mul(smBuffer.m_matrix, float4(worldPos, 1));
}

#endif // __SHADOW_MAP_LIB_HLSL__