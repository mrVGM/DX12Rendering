#ifndef __PSM_LIB_HLSL__
#define __PSM_LIB_HLSL__

#include "common_buffers_lib.hlsl"
#include "objects_lib.hlsl"

float4 CalculatePSM(CameraBuffer camBuff, PSMBuffer psmBuff, float3 worldPos)
{
	float4 camPersp = mul(camBuff.m_matrix, float4(worldPos, 1));
	camPersp /= camPersp.w;

	float4 lightPersp = mul(psmBuff.m_matrix, camPersp);
	return lightPersp;
}

#endif // __SHADOW_MAP_LIB_HLSL__