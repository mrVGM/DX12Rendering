#ifndef __PSM_LIB_HLSL__
#define __PSM_LIB_HLSL__

#include "common_buffers_lib.hlsl"
#include "objects_lib.hlsl"

float4 CalculatePSM(PSMBuffer psmBuff, float3 worldPos)
{
	float4 perspProj = mul(psmBuff.m_perspMatrix, float4(worldPos, 1));
	float4 orthoProj = mul(psmBuff.m_orthoMatrix, perspProj);

	return orthoProj;
}

#endif // __SHADOW_MAP_LIB_HLSL__