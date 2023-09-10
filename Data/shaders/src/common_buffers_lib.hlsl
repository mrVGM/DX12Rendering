#ifndef __COMMON_BUFFERS_LIB_HLSL__
#define __COMMON_BUFFERS_LIB_HLSL__

struct VertexInput3D
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
    float3 objectPosition : OBJECT_POSITION;
    float4 objectRotation : OBJECT_ROTATION;
    float3 objectScale : OBJECT_SCALE;
};

struct SkeletalMeshVertexInput3D
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : UV;
    float3 objectPosition : OBJECT_POSITION;
    float4 objectRotation : OBJECT_ROTATION;
    float3 objectScale : OBJECT_SCALE;
    
    int4 m_jointIndex1       :  JOINT_1_INDEX;
    int4 m_jointIndex2       :  JOINT_2_INDEX;
    float4 m_jointWeight1    :  JOINT_1_WEIGHT;
    float4 m_jointWeight2    :  JOINT_2_WEIGHT;
};

struct VertexInput2D
{
    float2 position : POSITION;
    float2 uv : UV;
};

struct CameraBuffer
{
    float4x4 m_matrix;
    float3 m_position;
    float m_placeholder1;

    float3 m_fwd;
    float m_placeholder2;

    float m_nearPlane;
    float m_farPlane;
    float m_fov;
    float m_aspect;

    int2 m_resolution;
    float2 placeholder3;
};

struct SingleSM
{
    float4x4 m_matrix;
    float4x4 m_inv;
    float4 m_position;
};

struct SMBuffer
{
    SingleSM m_sms[4];
    int m_resolution;
    float3 m_separators;
};

struct SMBuffer1
{
    float4x4 m_matrix;
    float4x4 m_inv;
    float3 m_position;
    float m_placeholder1;

    int m_resolution;
    float3 m_placeholder2[3];
};

#endif // __COMMON_BUFFERS_LIB_HLSL__