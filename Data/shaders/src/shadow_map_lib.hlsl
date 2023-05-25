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

float CalculateShadowMapBiasedDepth(SMBuffer smBuffer, float3 worldPos, float3 worldNormal)
{
    float2 uv = CalculateShadowMapUV(smBuffer, worldPos);
    float2 biasedUV = CalculateShadowMapUV(smBuffer, worldPos + worldNormal);

    float2 uvDir = biasedUV - uv;

    {
        float maxUVOffset = max(abs(uvDir.x), abs(uvDir.y));
        uvDir /= maxUVOffset;

        float halfPixel = 1.0 / smBuffer.m_resolution;
        uvDir *= halfPixel;
    }

    float3 biasDir;
    float3 biasOrigin;
    {
        float4 biasStart = mul(smBuffer.m_inv, float4(uv + uvDir, 0, 1));
        float4 biasEnd = mul(smBuffer.m_inv, float4(uv + uvDir, 1, 1));
        
        biasStart /= biasStart.w;
        biasEnd /= biasEnd.w;

        biasOrigin = biasStart;
        biasDir = biasEnd - biasOrigin;
    }

    float3 biasOriginOffset = worldPos - biasOrigin;

    float biasOriginMeasure = dot(biasOriginOffset, worldNormal);
    float biasDirMeasure = dot(biasDir, worldNormal);

    float factor = biasOriginMeasure / biasDirMeasure;

    float3 biasedPoint = biasOrigin + factor * biasDir;

    float biasedDepth = CalculateShadowMapDepth(smBuffer, biasedPoint);

    return biasedDepth + 0.001;
}

#endif // __SHADOW_MAP_LIB_HLSL__