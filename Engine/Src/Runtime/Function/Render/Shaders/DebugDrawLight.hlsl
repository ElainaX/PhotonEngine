#include "Includes/VertexSimple.hlsl"
#include "Includes/SimpleLight.hlsl"

struct VertexOutput
{
	float4 posClip : SV_POSITION;
};

cbuffer cbObject : register(b0)
{
	float4x4 gWorld;
}

cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gProjView;
	float4x4 gInvProjView;
	float3 gEyePos;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;
	
#ifdef MaxLights
	Light gLights[MaxLights];
#endif
}

VertexOutput VS(VertexInput vin)
{
	VertexOutput vout;
	float4 posw = mul(gWorld, float4(vin.pos, 1.0));
	vout.posClip = mul(gProjView, posw);
	return vout;
}

float4 PS(VertexOutput pin) : SV_Target
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

