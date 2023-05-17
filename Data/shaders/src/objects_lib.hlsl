#ifndef __OBJECTS_LIB_HLSL__
#define __OBJECTS_LIB_HLSL__

#include "quat_lib.hlsl"

void GetWorldPositonAndNormal(
    float3 position,
    float3 normal,
    float3 objectPosition,
    float4 objectRotation,
    float3 objectScale,
    
    out float3 worldPos,
    out float3 worldNormal)
{
    float3 scaledPos = objectScale * position;
    float3 rotatedPos = rotateVector(scaledPos, objectRotation);
    float3 rotatedNormal = rotateVector(normal, objectRotation);

    float3 pos = objectPosition + rotatedPos;
    worldPos = pos;
    worldNormal = rotatedNormal;
}

#endif // __OBJECTS_LIB_HLSL__