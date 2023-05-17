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

struct VertexInput2D
{
    float2 position : POSITION;
    float2 uv : UV;
};

struct CameraBuffer
{
    float4x4 m_matrix;
    float3 m_position;
    float m_placeholder;
};

struct SMBuffer
{
    float4x4 m_matrix;
    float3 m_position;
    float m_placeholder;
};

#endif // __COMMON_BUFFERS_LIB_HLSL__