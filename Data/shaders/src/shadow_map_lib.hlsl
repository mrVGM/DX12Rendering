#ifndef __SHADOW_MAP_LIB_HLSL__
#define __SHADOW_MAP_LIB_HLSL__

#include "common_buffers_lib.hlsl"
#include "objects_lib.hlsl"

float4 CalculateShadowMap(SMBuffer smBuffer, int index, float3 worldPos)
{
    SingleSM singleSM = smBuffer.m_sms[index];
    return mul(singleSM.m_matrix, float4(worldPos, 1));
}

float2 CalculateShadowMapUV(SMBuffer smBuffer, int index, float3 worldPos)
{
    float4 proj = CalculateShadowMap(smBuffer, index, worldPos);
    return proj / proj.w;
}

int GetSMIndex(SMBuffer smBuffer, float3 position)
{
    for (int i = 0; i < 4; ++i)
    {
        float2 uv = CalculateShadowMapUV(smBuffer, i, position);

        if (-1 <= uv.x && uv.x <= 1 && -1 <= uv.y && uv.y <= 1)
        {
            return i;
        }
    }

    return 3;
}

float2 CalculateShadowMapNormalizedUV(SMBuffer smBuffer, int index, float3 worldPos)
{
    float2 uv = CalculateShadowMapUV(smBuffer, index, worldPos);
    uv += 1;
    uv /= 2;
    return uv;
}

float CalculateShadowMapDepth(SMBuffer smBuffer, int index, float3 worldPos)
{
    float4 proj = CalculateShadowMap(smBuffer, index, worldPos);
    return proj.z / proj.w;
}

float CalculateShadowMapBiasedDepth(SMBuffer smBuffer, int index, float3 worldPos, float3 worldNormal)
{
    float2 uv = CalculateShadowMapUV(smBuffer, index, worldPos);
    float2 biasedUV = CalculateShadowMapUV(smBuffer, index, worldPos + worldNormal);

    float2 uvDir = biasedUV - uv;

    {
        float maxUVOffset = max(abs(uvDir.x), abs(uvDir.y));
        uvDir /= maxUVOffset;

        float pixel = 2.0 / smBuffer.m_resolution;
        uvDir *= pixel;
    }

    float3 biasDir;
    float3 biasOrigin;
    {
        SingleSM singleSM = smBuffer.m_sms[index];
        float4 biasStart = mul(singleSM.m_inv, float4(uv + uvDir, 0, 1));
        float4 biasEnd = mul(singleSM.m_inv, float4(uv + uvDir, 1, 1));
        
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

    float biasedDepth = CalculateShadowMapDepth(smBuffer, index, biasedPoint);

    return biasedDepth;
}

#endif // __SHADOW_MAP_LIB_HLSL__