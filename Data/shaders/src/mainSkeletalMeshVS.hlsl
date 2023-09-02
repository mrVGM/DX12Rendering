#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

StructuredBuffer<float4x4> v_skeletonBuffer     : register(t0);
StructuredBuffer<float4x4> v_skeletonPoseBuffer : register(t1);

struct PSInput
{
    float4 position         : SV_POSITION;
    float4 world_position   : WORLD_POSITION;
    float4 normal           : NORMAL;
    float2 uv               : UV;
};

PSInput VSMain(SkeletalMeshVertexInput3D vertexInput)
{
    PSInput result;

    float4x4 bindPoseMatrix = v_skeletonBuffer[0];
    float4 vertexPos = float4(0, 0, 0, 0);
    
    for (int i = 0; i < 4; ++i)
    {
        int jointIndex = vertexInput.m_jointIndex[i];
        float jointWeight = vertexInput.m_jointWeight[i];
        
        if (jointIndex < 0)
        {
            continue;
        }

        float4 res = mul(bindPoseMatrix, float4(vertexInput.position, 1));
        res = mul(v_skeletonBuffer[jointIndex + 1], res);
        res = mul(v_skeletonPoseBuffer[jointIndex], res);
        vertexPos += jointWeight * res;
    }
    
    
    float3 worldPos;
    float3 worldNormal;
    GetWorldPositonAndNormal(
        vertexPos,
        vertexInput.normal,
        vertexInput.objectPosition,
        vertexInput.objectRotation,
        vertexInput.objectScale,

        worldPos,
        worldNormal);
    
    result.position = mul(m_camBuff.m_matrix, float4(worldPos, 1));

    result.world_position = float4(worldPos, 1);
    result.normal = float4(worldNormal, 1);
    result.uv = vertexInput.uv;

    return result;
}
