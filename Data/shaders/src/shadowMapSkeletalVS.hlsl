#include "objects_lib.hlsl"
#include "common_buffers_lib.hlsl"
#include "shadow_map_lib.hlsl"

cbuffer CamBuff : register(b0)
{
    CameraBuffer m_camBuff;
};

cbuffer SMBuff : register(b1)
{
    SMBuffer m_smBuff;
};

cbuffer SMSlot : register(b2)
{
    int m_slot;
}

struct PSInput
{
    float4 position     : SV_POSITION;
    float depth       : DEPTH;
};

StructuredBuffer<float4x4> v_skeletonBuffer : register(t0);
StructuredBuffer<float4x4> v_skeletonPoseBuffer : register(t1);

static const float m_staticBias = 0.01;

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

        {
            float4 res = mul(bindPoseMatrix, float4(vertexInput.position, 1));
            res = mul(v_skeletonBuffer[jointIndex + 1], res);
            res = mul(v_skeletonPoseBuffer[jointIndex], res);
            vertexPos += jointWeight * res;
        }
    }
    
    
    float3 worldPos;
    float3 worldNormal;
    GetWorldPositonAndNormal(
        vertexPos.xyz,
        vertexInput.normal,
        vertexInput.objectPosition,
        vertexInput.objectRotation,
        vertexInput.objectScale,

        worldPos,
        worldNormal);
    
    result.position = CalculateShadowMap(m_smBuff, m_slot, worldPos);

    float depth = CalculateShadowMapDepth(m_smBuff, m_slot, worldPos);
    result.depth = depth;
    
    return result;
}
